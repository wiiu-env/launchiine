#ifndef __STRINGS_H_
#define __STRINGS_H_

#include <stdint.h>

static inline int32_t m_toupper(int32_t c) {
    return (c >= 'a' && c <= 'z') ? (c - 0x20) : c;
}

void* m_memcpy(void *dst, const void *src, uint32_t len);
void* m_memset(void *dst, int32_t val, uint32_t len);
int32_t m_memcmp (const void * ptr1, const void * ptr2, uint32_t num);

/* string functions */
int32_t m_strncasecmp(const char *s1, const char *s2, uint32_t max_len);
int32_t m_strncmp(const char *s1, const char *s2, uint32_t max_len);
int32_t m_strncpy(char *dst, const char *src, uint32_t max_size);
int32_t m_strlcpy(char *s1, const char *s2, uint32_t max_size);
int32_t m_strnlen(const char* str, uint32_t max_size);
int32_t m_strlen(const char* str);
const char *m_strcasestr(const char *str, const char *pattern);
long long m_strtoll(const char *str, char **end, int32_t base);

#endif // __STRINGS_H_
