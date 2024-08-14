#ifndef CORE_STACK_H
#define CORE_STACK_H

// this is where things get a little... f*cky....

// this function relocates the stack only from the kernel. Any local variables
// that you may want saved will get mangled and wont be usable. This should be
// called after initialization and before local variables are created. The stack
// is relocated to the region 0xBF800000->0xBFFFF000
void stack_relocate();

#endif
