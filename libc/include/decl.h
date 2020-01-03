//
// Created by Martin Miralles-Cordal on 8/9/2013.
//

#ifndef LAMBOS_LIBC_DECL_H
#define LAMBOS_LIBC_DECL_H

#undef __BEGIN_DECLS
#undef __END_DECLS

#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

#ifdef __cplusplus
#define __LIBC_CONSTEXPR constexpr
#else
#define __LIBC_CONSTEXPR /* empty */
#endif

#endif /* LAMBOS_LIBC_DECL_H */
