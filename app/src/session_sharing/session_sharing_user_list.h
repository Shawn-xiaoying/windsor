#ifndef SESSION_SHARING_USER_LIST_H
#define SESSION_SHARING_USER_LIST_H

bool  clear_session_sharing_user(uint8_t* userid);
bool  find_session_sharing_user(uint8_t* userid);
bool  insert_session_sharing_user(uint8_t* userid);
void  clear_all_sharing_user(void);
void  clear_session_all_sharing_user(void);
void  print_all_sharing_user(void);

#endif
