#!/usr/bin/env python3
"""Analyze a LambOS page-fault panic string using binutils tools.

Example:
  ./scripts/page_fault_report.py "Page Fault: cr2=0xFFC00004 eip=0xC0112498/in Kernel mode/"
"""

from __future__ import annotations

import argparse
import json
import re
import shlex
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Sequence, Tuple


class AnalysisError(RuntimeError):
    """Raised when user input or environment is not suitable for analysis."""


@dataclass
class PageFaultInfo:
    panic_string: str
    cr2: int
    eip: int
    flags: List[str]
    inferred_error_code: int
    inferred_bits: Dict[str, bool]


@dataclass
class SymbolEntry:
    address: int
    symbol_type: str
    name: str


@dataclass
class ToolRun:
    name: str
    command: List[str]
    returncode: int
    stdout: str
    stderr: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Parse a LambOS page-fault panic string and build a debug report.",
    )
    parser.add_argument(
        "panic",
        nargs="?",
        help="Panic string from PageFaultISR. If omitted, read from stdin.",
    )
    parser.add_argument(
        "--kernel",
        help="Path to kernel ELF/binary (default: auto-detect build/kernel/kernel.bin).",
    )
    parser.add_argument(
        "--build-dir",
        dest="build_dirs",
        action="append",
        help="Build directory to inspect for CMakeCache.txt and kernel.bin (repeatable).",
    )
    parser.add_argument(
        "--tool-prefix",
        help="Cross-tool prefix, e.g. i686-elf- or /opt/homebrew/bin/i686-elf-.",
    )
    parser.add_argument(
        "--objdump-window",
        type=lambda v: int(v, 0),
        default=0x60,
        help="Byte range on each side of EIP for disassembly (default: 0x60).",
    )
    parser.add_argument(
        "--output",
        help="Optional output report file path.",
    )
    parser.add_argument(
        "--format",
        choices=("text", "json"),
        default="text",
        help="Output format (default: text).",
    )
    return parser.parse_args(argv)


def normalize_hex(value: int) -> str:
    return f"0x{value:08X}"


def parse_hex(text: str) -> int:
    cleaned = text.strip().lower()
    if cleaned.startswith("0x"):
        cleaned = cleaned[2:]
    if not cleaned:
        raise AnalysisError("Encountered an empty hex value while parsing panic string.")
    return int(cleaned, 16)


def parse_panic_string(text: str) -> PageFaultInfo:
    panic_input = text.strip().lstrip("> ").strip()
    pattern = re.compile(
        r"Page Fault:\s*cr2=(0x[0-9a-fA-F]+|[0-9a-fA-F]+)\s+eip=(0x[0-9a-fA-F]+|[0-9a-fA-F]+)([^\n\r]*)",
        re.IGNORECASE,
    )
    match = pattern.search(panic_input)
    if not match:
        raise AnalysisError(
            "Unable to parse panic string. Expected format like: "
            "Page Fault: cr2=0x... eip=0x.../in Kernel mode/"
        )

    cr2 = parse_hex(match.group(1))
    eip = parse_hex(match.group(2))
    tail = match.group(3).strip()

    flags: List[str] = []
    if tail:
        for part in tail.split("/"):
            cleaned = part.strip()
            if cleaned:
                flags.append(cleaned)

    lower_flags = " ".join(flags).lower()
    inferred_bits = {
        "protection": "protection" in lower_flags,
        "write": "write access" in lower_flags,
        "user": "user mode" in lower_flags,
        "rsvd": "rsvd bits set" in lower_flags,
        "instruction_fetch": "nx violation" in lower_flags,
    }

    inferred_error_code = 0
    if inferred_bits["protection"]:
        inferred_error_code |= 0x01
    if inferred_bits["write"]:
        inferred_error_code |= 0x02
    if inferred_bits["user"]:
        inferred_error_code |= 0x04
    if inferred_bits["rsvd"]:
        inferred_error_code |= 0x08
    if inferred_bits["instruction_fetch"]:
        inferred_error_code |= 0x10

    return PageFaultInfo(
        panic_string=panic_input,
        cr2=cr2,
        eip=eip,
        flags=flags,
        inferred_error_code=inferred_error_code,
        inferred_bits=inferred_bits,
    )


def find_repo_root() -> Path:
    roots_to_try = [Path.cwd().resolve(), Path(__file__).resolve().parent.parent]
    for start in roots_to_try:
        for candidate in (start, *start.parents):
            if (candidate / "CMakeLists.txt").exists() and (candidate / "kernel").exists():
                return candidate
    return Path.cwd().resolve()


def parse_cmake_cache(cache_path: Path) -> Dict[str, str]:
    values: Dict[str, str] = {}
    if not cache_path.exists():
        return values

    cache_pattern = re.compile(r"^([^:#=]+):[^=]*=(.*)$")
    for raw_line in cache_path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("//") or line.startswith("#"):
            continue
        match = cache_pattern.match(line)
        if not match:
            continue
        key = match.group(1).strip()
        value = match.group(2).strip()
        values[key] = value
    return values


def resolve_build_dirs(repo_root: Path, build_dirs_arg: Optional[Iterable[str]]) -> List[Path]:
    provided = list(build_dirs_arg) if build_dirs_arg else ["build", "cmake-build-debug"]
    resolved: List[Path] = []
    for entry in provided:
        candidate = Path(entry).expanduser()
        if not candidate.is_absolute():
            candidate = repo_root / candidate
        candidate = candidate.resolve()
        if candidate.exists() and candidate not in resolved:
            resolved.append(candidate)
    return resolved


def resolve_kernel_path(repo_root: Path, build_dirs: Sequence[Path], kernel_arg: Optional[str]) -> Path:
    if kernel_arg:
        kernel_path = Path(kernel_arg).expanduser()
        if not kernel_path.is_absolute():
            kernel_path = repo_root / kernel_path
        kernel_path = kernel_path.resolve()
        if not kernel_path.exists():
            raise AnalysisError(f"Kernel binary not found at {kernel_path}")
        return kernel_path

    candidates: List[Path] = []
    for build_dir in build_dirs:
        candidates.append(build_dir / "kernel" / "kernel.bin")

    candidates.append(repo_root / "kernel.bin")

    for candidate in candidates:
        if candidate.exists():
            return candidate.resolve()

    raise AnalysisError(
        "Unable to find kernel binary. Expected one of: "
        + ", ".join(str(path) for path in candidates)
    )


def cache_paths_for_build_dirs(build_dirs: Sequence[Path]) -> List[Path]:
    paths: List[Path] = []
    for build_dir in build_dirs:
        cache_path = build_dir / "CMakeCache.txt"
        if cache_path.exists():
            paths.append(cache_path)
    return paths


def normalize_tool_prefix(prefix: str) -> str:
    return prefix if prefix.endswith("-") else f"{prefix}-"


def extract_prefix_from_tool(tool_path: str, suffix: str) -> Optional[str]:
    basename = Path(tool_path).name
    if basename.endswith(suffix):
        return str(Path(tool_path).with_name(basename[: -len(suffix)]))
    return None


def extract_prefix_from_compiler(compiler_path: str) -> Optional[str]:
    basename = Path(compiler_path).name
    for compiler_suffix in ("g++", "gcc"):
        if basename.endswith(compiler_suffix):
            return str(Path(compiler_path).with_name(basename[: -len(compiler_suffix)]))
    return None


def find_tools(
    build_dirs: Sequence[Path],
    user_tool_prefix: Optional[str],
) -> Tuple[Dict[str, str], str]:
    tool_keys = {
        "addr2line": "CMAKE_ADDR2LINE",
        "objdump": "CMAKE_OBJDUMP",
        "readelf": "CMAKE_READELF",
        "nm": "CMAKE_NM",
    }

    cache_files = cache_paths_for_build_dirs(build_dirs)

    for cache_file in cache_files:
        cache = parse_cmake_cache(cache_file)
        from_cache: Dict[str, str] = {}
        all_found = True
        for tool_name, cache_key in tool_keys.items():
            value = cache.get(cache_key, "")
            if not value or value.endswith("-NOTFOUND"):
                all_found = False
                break
            tool_path = Path(value)
            if not tool_path.exists():
                all_found = False
                break
            from_cache[tool_name] = str(tool_path)
        if all_found:
            return from_cache, f"Detected from {cache_file}"

    prefixes: List[str] = []
    if user_tool_prefix:
        prefixes.append(normalize_tool_prefix(user_tool_prefix))

    for cache_file in cache_files:
        cache = parse_cmake_cache(cache_file)
        addr2line_path = cache.get("CMAKE_ADDR2LINE")
        if addr2line_path:
            prefix = extract_prefix_from_tool(addr2line_path, "addr2line")
            if prefix:
                prefixes.append(prefix)

        compiler_path = cache.get("CMAKE_C_COMPILER")
        if compiler_path:
            prefix = extract_prefix_from_compiler(compiler_path)
            if prefix:
                prefixes.append(prefix)

    prefixes.extend(["i686-elf-", "i586-elf-", ""])

    deduped_prefixes: List[str] = []
    for prefix in prefixes:
        if prefix not in deduped_prefixes:
            deduped_prefixes.append(prefix)

    for prefix in deduped_prefixes:
        found: Dict[str, str] = {}
        for tool_name in tool_keys:
            executable_name = f"{prefix}{tool_name}"
            resolved = shutil.which(executable_name)
            if not resolved:
                found = {}
                break
            found[tool_name] = resolved
        if found:
            source = f"Resolved from tool prefix '{prefix or '<native>'}'"
            return found, source

    missing = ", ".join(tool_keys.keys())
    raise AnalysisError(
        "Unable to locate required tools: "
        f"{missing}. Provide --tool-prefix or ensure CMakeCache.txt exists and is configured."
    )


def run_tool(name: str, command: List[str]) -> ToolRun:
    completed = subprocess.run(
        command,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    return ToolRun(
        name=name,
        command=command,
        returncode=completed.returncode,
        stdout=completed.stdout.strip(),
        stderr=completed.stderr.strip(),
    )


def parse_symbols(nm_output: str) -> List[SymbolEntry]:
    symbols: List[SymbolEntry] = []
    symbol_pattern = re.compile(r"^([0-9A-Fa-f]+)\s+([A-Za-z])\s+(.+)$")
    for line in nm_output.splitlines():
        match = symbol_pattern.match(line.strip())
        if not match:
            continue
        symbols.append(
            SymbolEntry(
                address=int(match.group(1), 16),
                symbol_type=match.group(2),
                name=match.group(3),
            )
        )
    return symbols


def nearest_symbols(symbols: Sequence[SymbolEntry], address: int) -> Tuple[Optional[SymbolEntry], Optional[SymbolEntry]]:
    previous_symbol: Optional[SymbolEntry] = None
    next_symbol: Optional[SymbolEntry] = None
    for symbol in symbols:
        if symbol.address <= address:
            previous_symbol = symbol
            continue
        next_symbol = symbol
        break
    return previous_symbol, next_symbol


def find_section_for_address(readelf_sections_output: str, address: int) -> Optional[str]:
    # readelf -W -S line shape:
    # [ 1] .text PROGBITS c0100000 001000 01e000 00 AX 0 0 16
    section_pattern = re.compile(
        r"^\s*\[\s*\d+\]\s+(\S+)\s+\S+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+"
    )
    for line in readelf_sections_output.splitlines():
        match = section_pattern.match(line)
        if not match:
            continue
        section_name = match.group(1)
        section_addr = int(match.group(2), 16)
        section_size = int(match.group(3), 16)
        if section_size == 0:
            continue
        if section_addr <= address < section_addr + section_size:
            section_end = section_addr + section_size
            return f"{section_name} [{normalize_hex(section_addr)} .. {normalize_hex(section_end)} )"
    return None


def annotate_disassembly(disassembly: str, eip: int) -> str:
    address_pattern = re.compile(r"^\s*([0-9A-Fa-f]+):")
    annotated: List[str] = []
    for line in disassembly.splitlines():
        match = address_pattern.match(line)
        if match and int(match.group(1), 16) == eip:
            annotated.append(f">> {line}")
        else:
            annotated.append(line)
    return "\n".join(annotated).strip()


def heuristic_notes(info: PageFaultInfo) -> List[str]:
    notes: List[str] = []

    if info.cr2 < 0x1000:
        notes.append("Fault address is in the first page; this often indicates a null or near-null pointer dereference.")
    elif info.cr2 >= 0xFFC00000:
        notes.append("Fault address is in the top 4 MiB; this overlaps common self-map page-table regions on i386.")
    elif info.cr2 >= 0xC0000000:
        notes.append("Fault address is in higher-half kernel virtual space.")
    else:
        notes.append("Fault address is in low/user virtual space.")

    if info.inferred_bits["protection"]:
        notes.append("Error bit P=1: protection violation (page present but access disallowed).")
    else:
        notes.append("Error bit P=0: non-present page (mapping missing).")

    if info.inferred_bits["write"]:
        notes.append("Error bit W/R=1: write access triggered the fault.")
    else:
        notes.append("Error bit W/R=0: read access triggered the fault.")

    if info.inferred_bits["user"]:
        notes.append("Error bit U/S=1: fault occurred in user mode.")
    else:
        notes.append("Error bit U/S=0: fault occurred in supervisor/kernel mode.")

    if info.inferred_bits["rsvd"]:
        notes.append("Error bit RSVD=1: reserved bits were set in a paging structure; check page table entry construction.")

    if info.inferred_bits["instruction_fetch"]:
        notes.append("Error bit I/D=1: instruction fetch violated NX/XD permissions.")

    return notes


def format_command(command: Sequence[str]) -> str:
    return " ".join(shlex.quote(part) for part in command)


def format_tool_run(run: ToolRun) -> List[str]:
    lines = [f"Command: {format_command(run.command)}", f"Exit code: {run.returncode}"]
    if run.stdout:
        lines.append("Stdout:")
        lines.extend(f"  {line}" for line in run.stdout.splitlines())
    if run.stderr:
        lines.append("Stderr:")
        lines.extend(f"  {line}" for line in run.stderr.splitlines())
    if not run.stdout and not run.stderr:
        lines.append("Stdout/Stderr: <empty>")
    return lines


def tool_run_to_dict(run: ToolRun) -> Dict[str, Any]:
    return {
        "name": run.name,
        "command": run.command,
        "command_display": format_command(run.command),
        "exit_code": run.returncode,
        "stdout": run.stdout,
        "stderr": run.stderr,
    }


def format_tool_run_data(run_data: Dict[str, Any]) -> List[str]:
    lines = [f"Command: {run_data['command_display']}", f"Exit code: {run_data['exit_code']}"]
    if run_data["stdout"]:
        lines.append("Stdout:")
        lines.extend(f"  {line}" for line in str(run_data["stdout"]).splitlines())
    if run_data["stderr"]:
        lines.append("Stderr:")
        lines.extend(f"  {line}" for line in str(run_data["stderr"]).splitlines())
    if not run_data["stdout"] and not run_data["stderr"]:
        lines.append("Stdout/Stderr: <empty>")
    return lines


def symbol_to_dict(symbol: SymbolEntry, delta: int, delta_direction: str) -> Dict[str, Any]:
    return {
        "name": symbol.name,
        "type": symbol.symbol_type,
        "address": symbol.address,
        "address_hex": normalize_hex(symbol.address),
        "delta": delta,
        "delta_hex": f"0x{delta:X}",
        "delta_direction": delta_direction,
    }


def collect_analysis(
    info: PageFaultInfo,
    kernel_path: Path,
    tool_paths: Dict[str, str],
    tool_source: str,
    objdump_window: int,
) -> Dict[str, Any]:

    addr2line_eip = run_tool(
        "addr2line(eip)",
        [tool_paths["addr2line"], "-f", "-C", "-i", "-e", str(kernel_path), normalize_hex(info.eip)],
    )
    addr2line_prev = run_tool(
        "addr2line(eip-1)",
        [
            tool_paths["addr2line"],
            "-f",
            "-C",
            "-i",
            "-e",
            str(kernel_path),
            normalize_hex(max(info.eip - 1, 0)),
        ],
    )

    nm_run = run_tool("nm", [tool_paths["nm"], "-n", "-C", str(kernel_path)])
    symbols = parse_symbols(nm_run.stdout) if nm_run.returncode == 0 else []
    previous_symbol, next_symbol = nearest_symbols(symbols, info.eip) if symbols else (None, None)

    readelf_run = run_tool("readelf", [tool_paths["readelf"], "-W", "-S", str(kernel_path)])
    containing_section = (
        find_section_for_address(readelf_run.stdout, info.eip) if readelf_run.returncode == 0 else None
    )

    start_address = max(info.eip - objdump_window, 0)
    stop_address = info.eip + objdump_window
    objdump_run = run_tool(
        "objdump",
        [
            tool_paths["objdump"],
            "-d",
            "-C",
            "-l",
            f"--start-address={normalize_hex(start_address)}",
            f"--stop-address={normalize_hex(stop_address)}",
            str(kernel_path),
        ],
    )
    annotated_disassembly = annotate_disassembly(objdump_run.stdout, info.eip) if objdump_run.stdout else ""

    nearest_le = None
    if previous_symbol:
        nearest_le = symbol_to_dict(previous_symbol, info.eip - previous_symbol.address, "plus")

    nearest_gt = None
    if next_symbol:
        nearest_gt = symbol_to_dict(next_symbol, next_symbol.address - info.eip, "minus")

    objdump_annotated_run = ToolRun(
        objdump_run.name,
        objdump_run.command,
        objdump_run.returncode,
        annotated_disassembly,
        objdump_run.stderr,
    )

    return {
        "input": {
            "panic_string": info.panic_string,
            "cr2": info.cr2,
            "cr2_hex": normalize_hex(info.cr2),
            "eip": info.eip,
            "eip_hex": normalize_hex(info.eip),
            "flags": info.flags,
            "inferred_error_code": info.inferred_error_code,
            "inferred_error_code_hex": f"0x{info.inferred_error_code:02X}",
            "inferred_bits": info.inferred_bits,
        },
        "heuristics": heuristic_notes(info),
        "artifacts": {
            "kernel_binary": str(kernel_path),
            "tool_discovery": tool_source,
            "tools": tool_paths,
        },
        "symbol_context": {
            "nearest_le": nearest_le,
            "nearest_gt": nearest_gt,
            "containing_section": containing_section,
            "symbol_count": len(symbols),
        },
        "addr2line": {
            "eip": tool_run_to_dict(addr2line_eip),
            "eip_minus_1": tool_run_to_dict(addr2line_prev),
        },
        "disassembly": {
            "window_start": start_address,
            "window_start_hex": normalize_hex(start_address),
            "window_end": stop_address,
            "window_end_hex": normalize_hex(stop_address),
            "objdump": tool_run_to_dict(objdump_annotated_run),
        },
        "raw_context": {
            "readelf_sections": tool_run_to_dict(readelf_run),
            "nm_symbols": tool_run_to_dict(nm_run),
        },
        "agent_next_checks": [
            "Start at the nearest <= EIP symbol and inspect pointer values that could produce CR2.",
            "If section is .text but CR2 is near paging structures, inspect page-table edit paths and address-space activation.",
            "Compare EIP and EIP-1 addr2line results to catch boundary effects around call/branch instructions.",
        ],
    }


def render_text_report(analysis: Dict[str, Any]) -> str:
    input_data = analysis["input"]
    artifacts = analysis["artifacts"]
    tools = artifacts["tools"]
    symbol_context = analysis["symbol_context"]
    disassembly = analysis["disassembly"]

    report_lines: List[str] = []
    report_lines.append("Page Fault Analysis Report")
    report_lines.append("==========================")
    report_lines.append("")
    report_lines.append("Input")
    report_lines.append("-----")
    report_lines.append(f"Panic string: {input_data['panic_string']}")
    report_lines.append(f"cr2: {input_data['cr2_hex']}")
    report_lines.append(f"eip: {input_data['eip_hex']}")
    report_lines.append(f"Parsed flag fragments: {', '.join(input_data['flags']) if input_data['flags'] else '<none>'}")
    report_lines.append(f"Inferred page-fault error code: {input_data['inferred_error_code_hex']}")
    report_lines.append("")

    report_lines.append("Heuristics")
    report_lines.append("----------")
    for note in analysis["heuristics"]:
        report_lines.append(f"- {note}")
    report_lines.append("")

    report_lines.append("Artifacts")
    report_lines.append("---------")
    report_lines.append(f"Kernel binary: {artifacts['kernel_binary']}")
    report_lines.append(f"Tool discovery: {artifacts['tool_discovery']}")
    report_lines.append(f"addr2line: {tools['addr2line']}")
    report_lines.append(f"objdump: {tools['objdump']}")
    report_lines.append(f"readelf: {tools['readelf']}")
    report_lines.append(f"nm: {tools['nm']}")
    report_lines.append("")

    report_lines.append("Symbol/Section Context")
    report_lines.append("----------------------")
    nearest_le = symbol_context["nearest_le"]
    if nearest_le:
        report_lines.append(
            "Nearest <= EIP symbol: "
            f"{nearest_le['name']} ({nearest_le['type']}) "
            f"at {nearest_le['address_hex']} (+{nearest_le['delta_hex']})"
        )
    else:
        report_lines.append("Nearest <= EIP symbol: <not found>")

    nearest_gt = symbol_context["nearest_gt"]
    if nearest_gt:
        report_lines.append(
            "Nearest > EIP symbol: "
            f"{nearest_gt['name']} ({nearest_gt['type']}) "
            f"at {nearest_gt['address_hex']} (-{nearest_gt['delta_hex']})"
        )
    else:
        report_lines.append("Nearest > EIP symbol: <not found>")

    report_lines.append(f"Containing section: {symbol_context['containing_section'] or '<not found>'}")
    report_lines.append("")

    report_lines.append("addr2line")
    report_lines.append("---------")
    report_lines.extend(format_tool_run_data(analysis["addr2line"]["eip"]))
    report_lines.append("")
    report_lines.extend(format_tool_run_data(analysis["addr2line"]["eip_minus_1"]))
    report_lines.append("")

    report_lines.append("Disassembly Window")
    report_lines.append("------------------")
    report_lines.append(f"Window: {disassembly['window_start_hex']} .. {disassembly['window_end_hex']}")
    report_lines.extend(format_tool_run_data(disassembly["objdump"]))
    report_lines.append("")

    report_lines.append("Raw Context Commands")
    report_lines.append("--------------------")
    report_lines.extend(format_tool_run_data(analysis["raw_context"]["readelf_sections"]))
    report_lines.append("")

    report_lines.append("----------------")
    report_lines.append("Agent Next Checks")
    report_lines.append("----------------")
    for index, check in enumerate(analysis["agent_next_checks"], start=1):
        report_lines.append(f"{index}. {check}")

    return "\n".join(report_lines).rstrip() + "\n"


def render_json_report(analysis: Dict[str, Any]) -> str:
    return json.dumps(analysis, indent=2) + "\n"


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    panic_text = args.panic
    if panic_text is None:
        stdin_text = sys.stdin.read().strip()
        if not stdin_text:
            raise AnalysisError("No panic string provided. Pass it as an argument or via stdin.")
        panic_text = stdin_text

    info = parse_panic_string(panic_text)

    repo_root = find_repo_root()
    build_dirs = resolve_build_dirs(repo_root, args.build_dirs)
    if not build_dirs:
        raise AnalysisError(
            "No usable build directories found. Provide --build-dir pointing at a directory with CMakeCache.txt."
        )

    kernel_path = resolve_kernel_path(repo_root, build_dirs, args.kernel)
    tool_paths, tool_source = find_tools(build_dirs, args.tool_prefix)

    analysis = collect_analysis(info, kernel_path, tool_paths, tool_source, args.objdump_window)
    report = render_json_report(analysis) if args.format == "json" else render_text_report(analysis)

    if args.output:
        output_path = Path(args.output).expanduser()
        if not output_path.is_absolute():
            output_path = Path.cwd() / output_path
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(report, encoding="utf-8")
        print(report, end="")
        print(f"Report written to {output_path}", file=sys.stderr)
    else:
        print(report, end="")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main(sys.argv[1:]))
    except AnalysisError as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
