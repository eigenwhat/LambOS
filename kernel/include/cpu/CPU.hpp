#pragma once

namespace concepts {

template <typename T>
concept CPU = requires(T a) {
    a.install();
    a.enableInterrupts();
    a.disableInterrupts();
};

} // namespace concepts
