#ifndef HFIELD_H
#define HFIELD_H

extern unsigned int hfield_skipname();
extern int hfield_mort();
extern int hfield_known();
extern int hfield_valid();

#define H_MESSAGE 1
#define H_TEXT 2
#define H_SENDER 3
#define H_FROM 4
#define H_REPLYTO 5
#define H_TO 6
#define H_CC 7
#define H_BCC 8
#define H_DATE 9
#define H_MESSAGEID 10
#define H_SUBJECT 11
#define H_R_SENDER 12
#define H_R_FROM 13
#define H_R_REPLYTO 14
#define H_R_TO 15
#define H_R_CC 16
#define H_R_BCC 17
#define H_R_DATE 18
#define H_R_MESSAGEID 19
#define H_RETURNRECEIPTTO 20
#define H_ERRORSTO 21
#define H_APPARENTLYTO 22
#define H_RECEIVED 23
#define H_RETURNPATH 24
#define H_DELIVEREDTO 25
#define H_CONTENTLENGTH 26
#define H_CONTENTTYPE 27
#define H_CONTENTTRANSFERENCODING 28
#define H_NOTICEREQUESTEDUPONDELIVERYTO 29
#define H_NUM 30

#endif
