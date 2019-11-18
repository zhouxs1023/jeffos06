#ifndef __ERR
#define __ERR

/* NOTE If ret is negative then it is an error otherwise it returns a 
//	success code. */

/////
// Error codes
/////

#define NUM_ERR		22
#define NOERR		0
#define ERR		-1
#define ERRNOMEM	-2
#define ERRRESOURCE	-3
#define ERRMEMCORRUPT	-4
#define ERRFORMAT	-5
#define ERRNOTFOUND	-6
#define ERRTYPE		-7
#define ERRTIMEOUT	-8
#define ERRNOTSUPPORTED	-9
#define ERROUTOFRANGE	-10
#define ERRPRIV		-11
#define ERRNOTREADY	-12
#define ERRNONEFREE	-13
#define ERRARG		-14
#define ERRINVALID	-15
#define ERRNOTOPEN	-16
#define ERRALREADYDONE	-17
#define ERRVER		-18
#define ERROVERFLOW	-19
#define ERRINUSE	-20
#define ERRTOOBIG	-21

/////
// Success codes
/////

#define SFOUND		1
#define SNOTFOUND	2
#define SALT		3

/////
// Facility codes
/////

/* Bits 16-31 are reserved for the facility code. */
#define OWNOS		0x70
#define OWNNOMAD	0x70

/////
// Error Messages
/////

#define MAX_ERR_MSG	32
static const char err_msg[NUM_ERR][MAX_ERR_MSG] = {  "success",
					"error",
					"out of memory",
					"resource allocation",
					"memory corrupt!!!",
					"format",
					"not found",
					"type",
					"timeout",
					"not supported",
					"out of range",
					"access denied",
					"not ready",	
					"none free",
					"bad argument",
					"invalid",
					"not open",
					"already done",
					"incorrect version",
					"overflow",
					"in use",
					"too big" };

/////
// Return code object
/////

#ifdef __CPP_BINDING
class ret {
	private:
		int val;
	public:
		ret() { val=ERR; }
		ret(int src) { val=src; }
//		ret(const ret& src) { val=src; }
		ret operator=(const ret& src) { return val=src.val; }
		ret operator=(int src) { return val=src; }
		int operator==(const ret& cmp) { return val==cmp.val; }
		int operator!() { return val<0; } 
			// returns 1 if error, 0 if success
		int operator<(int cmp) {
			// TODO implement some severity check
			return (val<cmp); }
		int operator<(ret& cmp) { return (val<cmp.val); }
		operator int() const { return val; }	// cast
//		operator bool() { return (val<0); }
		const char *disp() {
			if(((val & 0x70) != 0x70) && ((val & 0x70) != 0x00))
				return "Unknown facility";
			return err_msg[-val]; }
};
#else
typedef int ret;
#endif /* __CPP_BINDING */

#endif /* __ERR */
