#ifndef ___FSM__H__
#define ___FSM__H__

typedef enum{
		IDLE,
		DOT_DASH_CNT,
		SAVE_DOT,
		SAVE_DASH,
		CHAR_CNT,
		SAVE_CHAR,
		SAVE_WORD,
		HANDLE_ERROR
}sm_state_t;

typedef enum{
		LOCKED,
		UNLOCKED
}sm_locked_status_t;

typedef enum{
	OK,
	ERR_CHAR_OVERFLOW,
	ERR_CHAR_INCOMPLETE,
	ERR_TIME_OVERFLOW,
	ERR_CHAR_UNKNOWN
}sm_status_t;

typedef struct sm_str{
		sm_state_t state;
		sm_locked_status_t lock;
		sm_status_t status;
}sm_t;

#endif