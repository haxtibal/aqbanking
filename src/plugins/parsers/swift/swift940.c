/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Fri Apr 02 2004
 copyright   : (C) 2004 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "swift940_p.h"

/* #include <aqhbci/aqhbci.h> */
#include <aqbanking/error.h>
#include <aqbanking/imexporter_be.h>

#include <gwenhywfar/text.h>
#include <gwenhywfar/debug.h>
#include <gwenhywfar/gwentime.h>
#include <gwenhywfar/waitcallback.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>



void AHB_SWIFT__HbciToUtf8(const char *p,
                           int size,
                           GWEN_BUFFER *buf) {
  while(*p) {
    unsigned int c;

    if (!size)
      break;

    c=(unsigned char)(*(p++));
    switch(c) {
    case 0xc4: /* AE */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x84);
      break;

    case 0xe4: /* ae */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa4);
      break;

    case 0xd6: /* OE */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x96);
      break;

    case 0xf6: /* oe */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xb6);
      break;

    case 0xdc: /* UE */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x9c);
      break;

    case 0xfc: /* ue */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xbc);
      break;

    case 0xdf: /* sz */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x9f);
      break;

    case 0xa7: /* section sign */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x67);
      break;

      /* english chars */
    case 0xa3: /* pound swign */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x63);
      break;

      /* french chars */
    case 0xc7: /* C cedille */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0x87);
      break;

    case 0xe0: /* a accent grave */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa0);
      break;

    case 0xe1: /* a accent aigu */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa1);
      break;

    case 0xe2: /* a accent circumflex */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa2);
      break;

    case 0xe7: /* c cedille */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa7);
      break;

    case 0xe8: /* e accent grave */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa8);
      break;

    case 0xe9: /* e accent aigu */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xa9);
      break;

    case 0xea: /* e accent circumflex */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xaa);
      break;

    case 0xec: /* i accent grave (never heard of this) */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xac);
      break;

    case 0xed: /* i accent aigu (never heard of this, either) */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xad);
      break;

    case 0xee: /* i accent circumflex (never heard of this, either) */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xae);
      break;

    case 0xf2: /* o accent grave */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xb2);
      break;

    case 0xf3: /* o accent aigu */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xb3);
      break;

    case 0xf4: /* o accent circumflex */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xb4);
      break;

    case 0xf9: /* u accent grave */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xb9);
      break;

    case 0xfa: /* u accent aigu */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xba);
      break;

    case 0xfb: /* u accent circumflex */
      GWEN_Buffer_AppendByte(buf, 0xc3);
      GWEN_Buffer_AppendByte(buf, 0xbb);
      break;

    default:
      GWEN_Buffer_AppendByte(buf, c);
    }
    if (size!=-1)
      size--;
  } /* while */
}



int AHB_SWIFT__SetCharValue(GWEN_DB_NODE *db,
                            GWEN_TYPE_UINT32 flags,
                            const char *name,
                            const char *s) {
  GWEN_BUFFER *vbuf;
  int rv;

  vbuf=GWEN_Buffer_new(0, strlen(s)+32, 0, 1);
  AHB_SWIFT__HbciToUtf8(s, -1, vbuf);
  rv=GWEN_DB_SetCharValue(db, flags, name, GWEN_Buffer_GetStart(vbuf));
  GWEN_Buffer_free(vbuf);
  return rv;
}


int AHB_SWIFT940_Parse_86(const AHB_SWIFT_TAG *tg,
                          GWEN_TYPE_UINT32 flags,
                          GWEN_DB_NODE *data,
                          GWEN_DB_NODE *cfg){
  const char *p;
  int isStructured;
  int code;

  p=AHB_SWIFT_Tag_GetData(tg);
  assert(p);
  isStructured=0;
  code=999;
  if (strlen(p)>3) {
    if (isdigit(p[0]) && isdigit(p[1]) && isdigit(p[2])) {
      /* starts with a three digit number */
      code=(((p[0]-'0')*100) + ((p[1]-'0')*10) + (p[2]-'0'));
      if (p[3]=='?')
	/* it is structured, get the code */
	isStructured=1;
      p+=3;
    }
  }

  if (isStructured) {
    const char *p2;
    char *s;
    int id;

    /* store code */
    GWEN_DB_SetIntValue(data, flags, "textkey", code);

    while(*p) {
      if (strlen(p)<3) {
	DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad field in :86: tag (%s)", p);
        GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                              "SWIFT: Bad field in :86: tag");
        return -1;
      }
      p++; /* skip '?' */
      /* read field id */
      if (*p==10)
        p++;
      if (!*p) {
	DBG_ERROR(AQBANKING_LOGDOMAIN, "Partial field id");
        GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                              "SWIFT: Partial field id");
        return -1;
      }
      id=((*p-'0')*10);
      p++;

      if (*p==10)
        p++;
      if (!*p) {
        DBG_ERROR(AQBANKING_LOGDOMAIN, "Partial field id");
        GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                              "SWIFT: Partial field id");
        return -1;
      }
      id+=(*p-'0');
      p++;

      p2=p;

      /* find end of field/beginning of next field */
      while(*p2 && *p2!='?') p2++;
      s=(char*)malloc(p2-p+1);
      memmove(s, p, p2-p+1);
      s[p2-p]=0;
      AHB_SWIFT_Condense(s);
      DBG_DEBUG(AQBANKING_LOGDOMAIN, "Current field is %02d (%s)", id, s);
      /* now id is the field id, s points to the field content */
      if (*s) {
        switch(id) {
        case 0: /* Buchungstext */
          AHB_SWIFT__SetCharValue(data, flags, "text", s);
          break;
        case 10: /* Primanota */
          AHB_SWIFT__SetCharValue(data, flags, "primanota", s);
          break;
    
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 60:
        case 61:
        case 62:
        case 63: /* Verwendungszweck */
          AHB_SWIFT__SetCharValue(data, flags, "purpose", s);
          break;
    
        case 30: /* BLZ Gegenseite */
          AHB_SWIFT__SetCharValue(data, flags, "remoteBankCode", s);
          break;
    
        case 31: /* Kontonummer Gegenseite */
          AHB_SWIFT__SetCharValue(data, flags, "remoteAccountNumber", s);
          break;
    
        case 32: 
        case 33: /* Name Auftraggeber */
          AHB_SWIFT__SetCharValue(data, flags, "remoteName", s);
          break;
    
        case 34: /* Textschluesselergaenzung */
          break;
    
        default: /* ignore all other fields (if any) */
          DBG_WARN(AQBANKING_LOGDOMAIN, "Unknown :86: field \"%02d\" (%s) (%s)", id, s,
                   AHB_SWIFT_Tag_GetData(tg));
          break;
        } /* switch */
      }
      p=p2;
      free(s);
    } /* while */
  } /* if structured */
  else {
    /* unstructured :86:, simply store as purpose line */
    AHB_SWIFT__SetCharValue(data, flags, "purpose", p);
  }

  return 0;
}



int AHB_SWIFT940_Parse_61(const AHB_SWIFT_TAG *tg,
                          GWEN_TYPE_UINT32 flags,
                          GWEN_DB_NODE *data,
                          GWEN_DB_NODE *cfg){
  const char *p;
  const char *p2;
  char *s;
  char buffer[32];
  unsigned int bleft;
  int d1a, d2a, d3a;
  int d1b, d2b, d3b;
  int neg;
  GWEN_TIME *ti;

  p=AHB_SWIFT_Tag_GetData(tg);
  assert(p);
  bleft=strlen(p);

  /* valuata date (M) */
  if (bleft<6) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Missing valuta date (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Missing valuta date");
    return -1;
  }
  d1a=((p[0]-'0')*10) + (p[1]-'0');
  d1a+=2000;
  d2a=((p[2]-'0')*10) + (p[3]-'0');
  d3a=((p[4]-'0')*10) + (p[5]-'0');
  ti=GWEN_Time_new(d1a, d2a-1, d3a, 12, 0, 0, 1);
  assert(ti);
  if (GWEN_Time_toDb(ti, GWEN_DB_GetGroup(data,
                                          GWEN_DB_FLAGS_OVERWRITE_GROUPS,
                                          "valutadate"))) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Error saving valuta date");
  }
  p+=6;
  bleft-=6;

  /* booking data (K) */
  if (*p && isdigit(*p)) {
    if (bleft<4) {
      DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad booking date (%s)", p);
      GWEN_WaitCallback_Log(GWEN_LoggerLevelInfo,
                            "SWIFT: Bad booking date");
      return -1;
    }
    d2b=((p[0]-'0')*10) + (p[1]-'0');
    d3b=((p[2]-'0')*10) + (p[3]-'0');
    /* use year from valutaDate.
     * However: if valuta date and booking date are in different years
     * the booking year might be too high.
     * We detect this case by comparing the months: If the booking month
     * is and the valuta month differ by more than 10 months then the year
     * of the booking date will be adjusted.
     */
    if (d2b-d2a>10) {
      /* booked before actually withdrawn */
      d1b=d1a-1;
    }
    else if (d2a-d2b>10) {
      /* withdrawn and booked later */
      d1b=d1a+1;
    }
    else
      d1b=d1a;

    ti=GWEN_Time_new(d1b, d2b-1, d3b, 12, 0, 0, 1);
    assert(ti);
    if (GWEN_Time_toDb(ti, GWEN_DB_GetGroup(data,
					    GWEN_DB_FLAGS_OVERWRITE_GROUPS,
					    "date"))) {
      DBG_ERROR(AQBANKING_LOGDOMAIN, "Error saving date");
    }
    p+=4;
    bleft-=4;
  }

  /* credit/debit mark (M) */
  if (bleft<2) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad value string (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Bad value string");
    return -1;
  }
  neg=0;
  if (*p=='R') {
    if (p[1]=='C' || p[1]=='c')
      neg=1;
    p+=2;
    bleft-=2;
  }
  else {
    if (*p=='D' || *p=='d')
      neg=1;
    p++;
    bleft--;
  }

  /* third character of currency (K) */
  if (bleft<1) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad data (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Bad currency");
    return -1;
  }
  if (!isdigit(*p)) {
    /* found third character, skip it */
    p++;
    bleft--;
  }

  /* value (M) */
  p2=p;
  while(*p2 && *p2!='N')
    p2++;
  if (p2==p || *p2!='N') {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad value (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Bad value");
    return -1;
  }
  s=(char*)malloc(p2-p+1+(neg?1:0));
  if (neg) {
    s[0]='-';
    memmove(s+1, p, p2-p+1);
    s[p2-p+1]=0;
  }
  else {
    memmove(s, p, p2-p+1);
    s[p2-p]=0;
  }
  AHB_SWIFT__SetCharValue(data, flags, "value/value", s);
  AHB_SWIFT__SetCharValue(data, flags,
                          "value/currency",
                          GWEN_DB_GetCharValue(cfg, "currency", 0, "EUR"));
  free(s);
  bleft-=p2-p;
  p=p2;

  /* skip 'N' */
  p++;
  bleft--;

  /* key (M) */
  if (bleft<3) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Missing booking key (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Missing booking key");
    return -1;
  }
  memmove(buffer, p, 3);
  buffer[3]=0;
  AHB_SWIFT__SetCharValue(data, flags, "key", buffer);
  p+=3;
  bleft-=3;

  /* customer reference (M) */
  p2=p;
  while(*p2 && *p2!='/' && *p2!=10) p2++;

  if (p2!=p) {
    s=(char*)malloc(p2-p+1);
    memmove(s, p, p2-p);
    s[p2-p]=0;
    AHB_SWIFT__SetCharValue(data, flags, "custref", s);
    free(s);
  }
  bleft-=p2-p;
  p=p2;

  /* bank reference (K) */
  if (*p=='/') {
    if (p[1]=='/') {
      /* found bank reference */
      p2=p+2;
      while(*p2 && *p2!='/' && *p2!=10) p2++;
      if (p2==p) {
        DBG_ERROR(AQBANKING_LOGDOMAIN, "Missing bank reference (%s)", p);
        GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                              "SWIFT: Missing bank reference");
        return -1;
      }
      s=(char*)malloc(p2-p+1);
      memmove(s, p, p2-p+1);
      s[p2-p]=0;
      AHB_SWIFT__SetCharValue(data, flags, "bankref", s);
      free(s);
      bleft-=p2-p;
      p=p2;
    }
    else {
      DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad data (%s)", p);
      GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                            "SWIFT: Bad bank reference");
      return -1;
    }
  }

  /* more information ? */
  if (*p==10) {
    /* yes... */
    p++;
    bleft--;

    while(*p) {
      /* read extra information */
      if (*p=='/') {
        if (bleft<6) {
          DBG_WARN(AQBANKING_LOGDOMAIN, "Unknown extra data, ignoring (%s)", p);
          return 0;
        }
        if (strncasecmp(p, "/OCMT/", 6)==0) {
          /* original value */
          p+=6;
          bleft-=6;
          /* get currency */
          memmove(buffer, p, 3);
          buffer[3]=0;
          AHB_SWIFT__SetCharValue(data, flags, "origvalue/currency", buffer);
          p+=3;
          bleft-=3;
          /* get value */
          p2=p;
          while(*p2 && *p2!='/') p2++;
          if (p2==p) {
            DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad original value (%s)", p);
            GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                                  "SWIFT: Bad original value");
            return -1;
          }
          s=(char*)malloc(p2-p+1);
          memmove(s, p, p2-p+1);
          s[p2-p]=0;
          AHB_SWIFT__SetCharValue(data, flags, "origvalue/value", s);
          free(s);
          bleft-=p2-p;
          p=p2;
        }
        else if (strncasecmp(p, "/CHGS/", 6)==0) {
          /* charges */
          p+=6;
          bleft-=6;
          /* get currency */
          memmove(buffer, p, 3);
          buffer[3]=0;
          AHB_SWIFT__SetCharValue(data, flags, "charges/currency", buffer);
          p+=3;
          bleft-=3;
          /* get value */
          p2=p;
          while(*p2 && *p2!='/') p2++;
          if (p2==p) {
            DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad charges value (%s)", p);
            GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                                  "SWIFT: Bad charges value");
            return -1;
          }
          s=(char*)malloc(p2-p+1);
          memmove(s, p, p2-p+1);
          s[p2-p]=0;
          AHB_SWIFT__SetCharValue(data, flags, "charges/value", s);
          free(s);
          bleft-=p2-p;
          p=p2;
        }
        else {
          DBG_WARN(AQBANKING_LOGDOMAIN, "Unknown extra data, ignoring (%s)", p);
          return 0;
        }
      }
      else {
        DBG_WARN(AQBANKING_LOGDOMAIN, "Bad extra data, ignoring (%s)", p);
        return 0;
      }
    } /* while */
  } /* if there is extra data */

  return 0;
}



int AHB_SWIFT940_Parse_6_0_2(const AHB_SWIFT_TAG *tg,
                             GWEN_TYPE_UINT32 flags,
                             GWEN_DB_NODE *data,
                             GWEN_DB_NODE *cfg){
  const char *p;
  const char *p2;
  char *s;
  char buffer[32];
  unsigned int bleft;
  int d1, d2, d3;
  int neg;
  GWEN_TIME *ti;

  p=AHB_SWIFT_Tag_GetData(tg);
  assert(p);
  bleft=strlen(p);

  /* credit/debit mark (M) */
  if (bleft<2) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad value string (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Bad value string");
    return -1;
  }
  neg=0;
  if (*p=='D' || *p=='d')
    neg=1;
  p++;
  bleft--;

  /* date (M) */
  if (bleft<6) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Missing date (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Missing date");
    return -1;
  }
  d1=((p[0]-'0')*10) + (p[1]-'0');
  d1+=2000;
  d2=((p[2]-'0')*10) + (p[3]-'0');
  d3=((p[4]-'0')*10) + (p[5]-'0');

  ti=GWEN_Time_new(d1, d2-1, d3, 12, 0, 0, 1);
  assert(ti);
  if (GWEN_Time_toDb(ti, GWEN_DB_GetGroup(data,
                                          GWEN_DB_FLAGS_OVERWRITE_GROUPS,
					  "date"))) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Error saving date");
  }

  p+=6;
  bleft-=6;

  /* currency (M) */
  if (bleft<3) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Missing currency (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Missing currency");
    return -1;
  }
  memmove(buffer, p, 3);
  buffer[3]=0;
  AHB_SWIFT__SetCharValue(data, flags, "value/currency", buffer);
  p+=3;
  bleft-=3;

  /* value (M) */
  p2=p;
  while(*p2) p2++;
  if (p2==p) {
    DBG_ERROR(AQBANKING_LOGDOMAIN, "Bad value (%s)", p);
    GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                          "SWIFT: Bad value");
    return -1;
  }
  s=(char*)malloc(p2-p+1+(neg?1:0));
  if (neg) {
    s[0]='-';
    memmove(s+1, p, p2-p+1);
    s[p2-p+1]=0;
  }
  else {
    memmove(s, p, p2-p+1);
    s[p2-p]=0;
  }
  AHB_SWIFT__SetCharValue(data, flags, "value/value", s);
  free(s);
  bleft-=p2-p;
  p=p2;

  return 0;
}



int AHB_SWIFT940_Import(GWEN_BUFFEREDIO *bio,
                        AHB_SWIFT_TAG_LIST *tl,
                        GWEN_TYPE_UINT32 flags,
                        GWEN_DB_NODE *data,
                        GWEN_DB_NODE *cfg){
  AHB_SWIFT_TAG *tg;
  GWEN_DB_NODE *dbDay;
  GWEN_DB_NODE *dbTransaction;
  int tagCount;

  dbDay=0;
  dbTransaction=0;
  tagCount=0;

  GWEN_WaitCallback_SetProgressTotal(AHB_SWIFT_Tag_List_GetCount(tl));

  tg=AHB_SWIFT_Tag_List_First(tl);
  while(tg) {
    const char *id;

    id=AHB_SWIFT_Tag_GetId(tg);
    assert(id);

    if (strcasecmp(id, "60F")==0) { /* StartSaldo */
      GWEN_DB_NODE *dbSaldo;

      /* start a new day */
      dbDay=GWEN_DB_GetGroup(data, GWEN_PATH_FLAGS_CREATE_GROUP, "day");
      dbTransaction=0;
      DBG_INFO(AQBANKING_LOGDOMAIN, "Starting new day");
      dbSaldo=GWEN_DB_GetGroup(dbDay, GWEN_PATH_FLAGS_CREATE_GROUP,
                               "StartSaldo");
      if (AHB_SWIFT940_Parse_6_0_2(tg, flags, dbSaldo, cfg)) {
        DBG_INFO(AQBANKING_LOGDOMAIN, "Error in tag");
        return -1;
      }
    }
    else if (strcasecmp(id, "62F")==0) { /* EndSaldo */
      GWEN_DB_NODE *dbSaldo;

      /* end current day */
      dbTransaction=0;
      if (!dbDay) {
        DBG_WARN(AQBANKING_LOGDOMAIN, "Your bank does not send an opening saldo");
        dbDay=GWEN_DB_GetGroup(data, GWEN_PATH_FLAGS_CREATE_GROUP, "day");
      }
      dbSaldo=GWEN_DB_GetGroup(dbDay, GWEN_PATH_FLAGS_CREATE_GROUP,
                               "EndSaldo");
      if (AHB_SWIFT940_Parse_6_0_2(tg, flags, dbSaldo, cfg)) {
        DBG_INFO(AQBANKING_LOGDOMAIN, "Error in tag");
        return -1;
      }
      dbDay=0;
    }
    else if (strcasecmp(id, "61")==0) {
      if (!dbDay) {
        DBG_WARN(AQBANKING_LOGDOMAIN, "Your bank does not send an opening saldo");
        dbDay=GWEN_DB_GetGroup(data, GWEN_PATH_FLAGS_CREATE_GROUP, "day");
      }

      DBG_INFO(AQBANKING_LOGDOMAIN, "Creating new transaction");
      dbTransaction=GWEN_DB_GetGroup(dbDay, GWEN_PATH_FLAGS_CREATE_GROUP,
                                     "transaction");
      if (AHB_SWIFT940_Parse_61(tg, flags, dbTransaction, cfg)) {
        DBG_INFO(AQBANKING_LOGDOMAIN, "Error in tag");
        return -1;
      }
    }
    else if (strcasecmp(id, "86")==0) {
      if (!dbTransaction) {
        DBG_WARN(AQBANKING_LOGDOMAIN, "Bad sequence of tags (86 before 61), ignoring");
      }
      else {
        if (AHB_SWIFT940_Parse_86(tg, flags, dbTransaction, cfg)) {
          DBG_INFO(AQBANKING_LOGDOMAIN, "Error in tag");
          return -1;
        }
      }
    }

    tagCount++;
    if (GWEN_WaitCallbackProgress(tagCount)==GWEN_WaitCallbackResult_Abort){
      DBG_INFO(AQBANKING_LOGDOMAIN, "User aborted");
      GWEN_WaitCallback_Log(GWEN_LoggerLevelError,
                            "SWIFT: User aborted");
      return AB_ERROR_USER_ABORT;
    }
    tg=AHB_SWIFT_Tag_List_Next(tg);
  } /* while */

  return 0;
}








