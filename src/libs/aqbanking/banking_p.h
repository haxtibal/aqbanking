/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Mon Mar 01 2004
 copyright   : (C) 2004 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/


#ifndef AQBANKING_BANKING_P_H
#define AQBANKING_BANKING_P_H

#define AB_BANKING_MAX_PIN_TRY 10

#define AB_BANKING_WCB_GENERIC "AB_BANKING_WCB_GENERIC"

#include "banking_l.h"
#include "account_l.h"
#include "job_l.h"
#include "imexporter_l.h"
#include "pin_l.h"
#include "bankinfoplugin_l.h"

#include <gwenhywfar/plugin.h>
#include <gwenhywfar/nettransportssl.h>
#include <gwenhywfar/waitcallback.h>


struct AB_BANKING {
  GWEN_INHERIT_ELEMENT(AB_BANKING)
  char *appName;
  char *appEscName;
  int appExtensions;

  char *dataDir;

  AB_JOB_LIST *enqueuedJobs;
  AB_ACCOUNT_LIST *accounts;

  GWEN_STRINGLIST *activeProviders;

  char *configFile;

  GWEN_DB_NODE *data;
  GWEN_DB_NODE *dbTempConfig;
  GWEN_DB_NODE *dbProfiles;

  AB_PROVIDER_LIST *providers;
  AB_IMEXPORTER_LIST *imexporters;
  AB_BANKINFO_PLUGIN_LIST *bankInfoPlugins;

  AB_BANKING_MESSAGEBOX_FN messageBoxFn;
  AB_BANKING_INPUTBOX_FN inputBoxFn;
  AB_BANKING_SHOWBOX_FN showBoxFn;
  AB_BANKING_HIDEBOX_FN hideBoxFn;
  AB_BANKING_PROGRESS_START_FN progressStartFn;
  AB_BANKING_PROGRESS_ADVANCE_FN progressAdvanceFn;
  AB_BANKING_PROGRESS_LOG_FN progressLogFn;
  AB_BANKING_PROGRESS_END_FN progressEndFn;
  AB_BANKING_PRINT_FN printFn;

  AB_BANKING_GETPIN_FN getPinFn;
  AB_BANKING_SETPINSTATUS_FN setPinStatusFn;
  AB_BANKING_GETTAN_FN getTanFn;
  AB_BANKING_SETTANSTATUS_FN setTanStatusFn;
  int pinCacheEnabled;
  int alwaysAskForCert;

  AB_PIN_LIST *pinList;

  GWEN_PLUGIN_MANAGER *pluginManagerBankInfo;
  GWEN_PLUGIN_MANAGER *pluginManagerProvider;
  GWEN_PLUGIN_MANAGER *pluginManagerImExporter;
  void *user_data;

  GWEN_WAITCALLBACK *waitCallback;

  int progressNestingLevel;
  GWEN_TYPE_UINT32 lastProgressId;
};



AB_PROVIDER *AB_Banking_FindProvider(AB_BANKING *ab, const char *name);
int AB_Banking__MergeInAccount(AB_BANKING *ab, AB_ACCOUNT *a);

AB_IMEXPORTER *AB_Banking_FindImExporter(AB_BANKING *ab, const char *name);

void AB_Banking__AddJobDir(const AB_BANKING *ab,
                           const char *as,
                           GWEN_BUFFER *buf);
void AB_Banking__AddJobPath(const AB_BANKING *ab,
			    const char *as,
                            GWEN_TYPE_UINT32 jid,
                            GWEN_BUFFER *buf);

int AB_Banking__OpenFile(const char *s, int wr);
int AB_Banking__CloseFile(int fd);


int AB_Banking__OpenJobAs(AB_BANKING *ab,
                          GWEN_TYPE_UINT32 jid,
                          const char *as,
                          int wr);
int AB_Banking__CloseJob(const AB_BANKING *ab, int fd);

AB_JOB *AB_Banking__LoadJobFile(AB_BANKING *ab, const char *s);

AB_JOB *AB_Banking__LoadJobAs(AB_BANKING *ab,
                              GWEN_TYPE_UINT32 jid,
                              const char *as);
int AB_Banking__SaveJobAs(AB_BANKING *ab,
                          AB_JOB *j,
			  const char *as);

AB_JOB_LIST2 *AB_Banking__LoadJobsAs(AB_BANKING *ab, const char *as);

int AB_Banking__UnlinkJobAs(AB_BANKING *ab,
                            AB_JOB *j,
                            const char *as);


int AB_Banking__GetAppConfigFileName(AB_BANKING *ab, GWEN_BUFFER *buf);
int AB_Banking__LoadAppData(AB_BANKING *ab);
int AB_Banking__SaveAppData(AB_BANKING *ab);


int AB_Banking__ReadImExporterProfiles(AB_BANKING *ab,
                                       const char *path,
                                       GWEN_DB_NODE *db);


int AB_Banking__GetProviderConfigFileName(AB_BANKING *ab,
                                          const char *name,
                                          GWEN_BUFFER *buf);
int AB_Banking__LoadProviderData(AB_BANKING *ab,
                                 const char *name);


int AB_Banking__SaveProviderData(AB_BANKING *ab,
                                 const char *name,
                                 int del);

int AB_Banking_InitProvider(AB_BANKING *ab, AB_PROVIDER *pro);
int AB_Banking_FiniProvider(AB_BANKING *ab, AB_PROVIDER *pro);

int AB_Banking__ExecuteQueue(AB_BANKING *ab, AB_JOB_LIST *jl);


AB_ACCOUNT *AB_Banking__GetAccount(AB_BANKING *ab,
                                   const char *accountId);

int AB_Banking__HashPin(AB_PIN *p);
int AB_Banking__SaveBadPins(AB_BANKING *ab);
int AB_Banking__CheckBadPin(AB_BANKING *ab, AB_PIN *p);

int AB_Banking__GetPin(AB_BANKING *ab,
                       GWEN_TYPE_UINT32 flags,
                       const char *token,
                       const char *title,
                       const char *text,
                       char *buffer,
                       int minLen,
                       int maxLen);

int AB_Banking__GetDebuggerPath(AB_BANKING *ab,
                                const char *backend,
                                GWEN_BUFFER *pbuf);
int AB_Banking__GetWizardPath(AB_BANKING *ab,
                              const char *backend,
                              GWEN_BUFFER *pbuf);

int AB_Banking__isSameDay(const GWEN_TIME *t1, const GWEN_TIME *t2);
void AB_Banking__RemoveDuplicateJobs(AB_BANKING *ab, AB_JOB_LIST2 *jl);

GWEN_NETTRANSPORTSSL_ASKADDCERT_RESULT
  AB_Banking_AskAddCert(GWEN_NETTRANSPORT *tr,
                        GWEN_DB_NODE *cert,
                        void *user_data);


#endif /* AQBANKING_BANKING_P_H */
