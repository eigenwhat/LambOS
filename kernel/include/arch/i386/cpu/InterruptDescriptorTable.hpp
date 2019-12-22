//
// Created by Martin Miralles-Cordal on 8/12/2013.
//

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include "RegisterTable.h"
#include "InterruptNumber.hpp"

extern "C" void set_idt(void *idt, size_t size);

#define IDT_SIZE 256

struct InterruptServiceRoutine
{
    virtual ~InterruptServiceRoutine() {}
    virtual void operator()(RegisterTable &) = 0;
};

class IDTEntry
{
  public:
    IDTEntry() : _offset(0), _selector(0), _typeAttr(0) {}
    IDTEntry(uint32_t offset, uint8_t type_attr) : _offset(offset), _selector(0x08), _typeAttr(type_attr) {}
    IDTEntry(uint16_t selector, uint32_t offset, uint8_t type_attr)
            : _offset(offset), _selector(selector), _typeAttr(type_attr) {}

    IDTEntry(uint16_t selector, void(*isrFn)(), uint8_t type_attr) : IDTEntry(selector, uint32_t(isrFn), type_attr) {}

    friend class InterruptDescriptorTable;

  private:
    uint32_t _offset;
    uint16_t _selector;
    uint8_t _typeAttr;
};

class InterruptDescriptorTable
{
  public:
    static std::uint64_t EncodeEntry(IDTEntry source);

    void encodeEntry(uint8_t entryNumber, IDTEntry source);
    void encodeISRs();
    void install() { set_idt(this->idt, sizeof(uint64_t) * IDT_SIZE); }

    void callISR(InterruptNumber interruptNumber, RegisterTable &registers)
    {
        (*isr[static_cast<int>(interruptNumber)])(registers);
    }

    void setISR(InterruptNumber interruptNumber, InterruptServiceRoutine *routine)
    {
        isr[static_cast<int>(interruptNumber)] = routine;
    }

  private:
    InterruptServiceRoutine *isr[IDT_SIZE];
    uint64_t idt[IDT_SIZE];
};
