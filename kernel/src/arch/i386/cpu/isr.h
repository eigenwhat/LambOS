#ifndef ISR_H
#define ISR_H

#include <decl.h>
#define ISR_FUNCTION_NAME(num) isr##num
#define ISR_FUNCTION_DEF(num) void ISR_FUNCTION_NAME(num) ()

#define ISR_FUNCTION_DEFS_5(n1, n2, n3, n4, n5) ISR_FUNCTION_DEF(n1); \
ISR_FUNCTION_DEF(n2); \
ISR_FUNCTION_DEF(n3); \
ISR_FUNCTION_DEF(n4); \
ISR_FUNCTION_DEF(n5)

#define ISR_FUNCTION_DEFS_10(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10) ISR_FUNCTION_DEFS_5(n1, n2, n3, n4, n5); \
ISR_FUNCTION_DEFS_5(n6, n7, n8, n9, n10)

__BEGIN_DECLS

ISR_FUNCTION_DEFS_10(0,1,2,3,4,5,6,7,8,9);
ISR_FUNCTION_DEFS_10(10,11,12,13,14,15,16,17,18,19);
ISR_FUNCTION_DEFS_10(20,21,22,23,24,25,26,27,28,29);
ISR_FUNCTION_DEFS_10(30,31,32,33,34,35,36,37,38,39);
ISR_FUNCTION_DEFS_10(40,41,42,43,44,45,46,47,48,49);
ISR_FUNCTION_DEFS_10(50,51,52,53,54,55,56,57,58,59);
ISR_FUNCTION_DEFS_10(60,61,62,63,64,65,66,67,68,69);
ISR_FUNCTION_DEFS_10(70,71,72,73,74,75,76,77,78,79);
ISR_FUNCTION_DEFS_10(80,81,82,83,84,85,86,87,88,89);
ISR_FUNCTION_DEFS_10(90,91,92,93,94,95,96,97,98,99);
ISR_FUNCTION_DEFS_10(100,101,102,103,104,105,106,107,108,109);
ISR_FUNCTION_DEFS_10(110,111,112,113,114,115,116,117,118,119);
ISR_FUNCTION_DEFS_10(120,121,122,123,124,125,126,127,128,129);
ISR_FUNCTION_DEFS_10(130,131,132,133,134,135,136,137,138,139);
ISR_FUNCTION_DEFS_10(140,141,142,143,144,145,146,147,148,149);
ISR_FUNCTION_DEFS_10(150,151,152,153,154,155,156,157,158,159);
ISR_FUNCTION_DEFS_10(160,161,162,163,164,165,166,167,168,169);
ISR_FUNCTION_DEFS_10(170,171,172,173,174,175,176,177,178,179);
ISR_FUNCTION_DEFS_10(180,181,182,183,184,185,186,187,188,189);
ISR_FUNCTION_DEFS_10(190,191,192,193,194,195,196,197,198,199);
ISR_FUNCTION_DEFS_10(200,201,202,203,204,205,206,207,208,209);
ISR_FUNCTION_DEFS_10(210,211,212,213,214,215,216,217,218,219);
ISR_FUNCTION_DEFS_10(220,221,222,223,224,225,226,227,228,229);
ISR_FUNCTION_DEFS_10(230,231,232,233,234,235,236,237,238,239);
ISR_FUNCTION_DEFS_10(240,241,242,243,244,245,246,247,248,249);
ISR_FUNCTION_DEFS_5(250,251,252,253,254);
ISR_FUNCTION_DEF(255);

__END_DECLS

#ifdef __cplusplus
using ISRFunction = void(*)();
constexpr inline ISRFunction kIsrs[256] = {
        isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9,
        isr10, isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19,
        isr20, isr21, isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29,
        isr30, isr31, isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39,
        isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47, isr48, isr49,
        isr50, isr51, isr52, isr53, isr54, isr55, isr56, isr57, isr58, isr59,
        isr60, isr61, isr62, isr63, isr64, isr65, isr66, isr67, isr68, isr69,
        isr70, isr71, isr72, isr73, isr74, isr75, isr76, isr77, isr78, isr79,
        isr80, isr81, isr82, isr83, isr84, isr85, isr86, isr87, isr88, isr89,
        isr90, isr91, isr92, isr93, isr94, isr95, isr96, isr97, isr98, isr99,
        isr100, isr101, isr102, isr103, isr104, isr105, isr106, isr107, isr108, isr109,
        isr110, isr111, isr112, isr113, isr114, isr115, isr116, isr117, isr118, isr119,
        isr120, isr121, isr122, isr123, isr124, isr125, isr126, isr127, isr128, isr129,
        isr130, isr131, isr132, isr133, isr134, isr135, isr136, isr137, isr138, isr139,
        isr140, isr141, isr142, isr143, isr144, isr145, isr146, isr147, isr148, isr149,
        isr150, isr151, isr152, isr153, isr154, isr155, isr156, isr157, isr158, isr159,
        isr160, isr161, isr162, isr163, isr164, isr165, isr166, isr167, isr168, isr169,
        isr170, isr171, isr172, isr173, isr174, isr175, isr176, isr177, isr178, isr179,
        isr180, isr181, isr182, isr183, isr184, isr185, isr186, isr187, isr188, isr189,
        isr190, isr191, isr192, isr193, isr194, isr195, isr196, isr197, isr198, isr199,
        isr200, isr201, isr202, isr203, isr204, isr205, isr206, isr207, isr208, isr209,
        isr210, isr211, isr212, isr213, isr214, isr215, isr216, isr217, isr218, isr219,
        isr220, isr221, isr222, isr223, isr224, isr225, isr226, isr227, isr228, isr229,
        isr230, isr231, isr232, isr233, isr234, isr235, isr236, isr237, isr238, isr239,
        isr240, isr241, isr242, isr243, isr244, isr245, isr246, isr247, isr248, isr249,
        isr250, isr251, isr252, isr253, isr254, isr255,
};
#endif

#endif
