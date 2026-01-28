#ifndef AUTH_H
#define AUTH_H

int auth_jwt_generate(const char *username, const char *role, char *out_token, size_t max_len);
int auth_jwt_verify(const char *token);

#endif /* AUTH_H */