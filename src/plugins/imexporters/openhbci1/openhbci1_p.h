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


#ifndef AQHBCI_IMEX_OHBCI1_P_H
#define AQHBCI_IMEX_OHBCI1_P_H


#include <gwenhywfar/dbio.h>
#include <aqbanking/imexporter_be.h>


typedef struct AH_IMEXPORTER_OPENHBCI1 AH_IMEXPORTER_OPENHBCI1;
struct AH_IMEXPORTER_OPENHBCI1 {
  GWEN_DB_NODE *dbData;
  GWEN_DBIO *dbio;
};


AQBANKING_EXPORT
GWEN_PLUGIN *imexporters_openhbci1_factory(GWEN_PLUGIN_MANAGER *pm,
					   const char *name,
					   const char *fileName);

static
AB_IMEXPORTER *AB_Plugin_ImExporterOpenHBCI1_Factory(GWEN_PLUGIN *pl,
						     AB_BANKING *ab,
						     GWEN_DB_NODE *db);

static
void GWENHYWFAR_CB AH_ImExporterOpenHBCI1_FreeData(void *bp, void *p);

static
int AH_ImExporterOpenHBCI1_Import(AB_IMEXPORTER *ie,
                                  AB_IMEXPORTER_CONTEXT *ctx,
                                  GWEN_IO_LAYER *bio,
				  GWEN_DB_NODE *params,
				  uint32_t guiid);

static
int AH_ImExporterOpenHBCI1_Export(AB_IMEXPORTER *ie,
                                  AB_IMEXPORTER_CONTEXT *ctx,
                                  GWEN_IO_LAYER *io,
				  GWEN_DB_NODE *params,
				  uint32_t guiid);

static
int AH_ImExporterOpenHBCI1_CheckFile(AB_IMEXPORTER *ie, const char *fname, uint32_t guiid);

static
int AH_ImExporterOpenHBCI1__ImportFromGroup(AB_IMEXPORTER_CONTEXT *ctx,
                                            GWEN_DB_NODE *db,
					    GWEN_DB_NODE *dbParams,
					    uint32_t guiid);

#endif /* AQHBCI_IMEX_OHBCI1_P_H */
