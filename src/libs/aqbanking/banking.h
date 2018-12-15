/***************************************************************************
 begin       : Mon Mar 01 2004
 copyright   : (C) 2018 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 * This file is part of the project "AqBanking".                           *
 * Please see toplevel file COPYING of that project for license details.   *
 ***************************************************************************/

/** @file 
 * @short The main interface of the aqbanking library
 */

#ifndef AQBANKING_BANKING_H
#define AQBANKING_BANKING_H


/** @addtogroup G_AB_BANKING Main Interface
 */
/*@{*/

/**
 * Object to be operated on by functions in this group (@ref AB_BANKING).
 */
typedef struct AB_BANKING AB_BANKING;
/*@}*/



#include <gwenhywfar/types.h>
#include <gwenhywfar/dialog.h>

#include <aqbanking/error.h> /* for AQBANKING_API */
#include <aqbanking/version.h>

/* outsourced */
#include <aqbanking/banking_imex.h>
#include <aqbanking/banking_bankinfo.h>
#include <aqbanking/banking_online.h>
#include <aqbanking/banking_transaction.h>
#include <aqbanking/banking_dialogs.h>



#ifdef __cplusplus
extern "C" {
#endif



/** @addtogroup G_AB_BANKING Main Interface
 *
 * @short This group contains the main API function group.
 *
 * <p>
 * A program should first call @ref AB_Banking_Init to allow AqBanking
 * to load its configuration files and initialize itself.
 * </p>
 * After that you may call any other function of this group (most likely
 * the program will request a list of managed account via
 * @ref AB_Banking_GetAccountSpecs).
 * </p>
 * <p>
 * When the program has finished its work it should call @ref AB_Banking_Fini
 * as the last function of AqBanking (just before calling
 * @ref AB_Banking_free).
 * </p>
 */
/*@{*/



/** @name Extensions supported by the application
 *
 */
/*@{*/
#define AB_BANKING_EXTENSION_NONE             0x00000000
/*@}*/


/**
 * This object is prepared to be inherited (using @ref GWEN_INHERIT_SETDATA).
 */
GWEN_INHERIT_FUNCTION_LIB_DEFS(AB_BANKING, AQBANKING_API)



/** @name Constructor, Destructor, Init, Fini
 *
 */
/*@{*/

/**
 * <p>
 * Creates an instance of AqBanking. Though AqBanking is quite object
 * oriented (and thus allows multiple instances of AB_BANKING to co-exist)
 * you should avoid having multiple AB_BANKING objects in parallel.
 * </p>
 * <p>
 * This is just because the backends are loaded dynamically and might not like
 * to be used with multiple instances of AB_BANKING in parallel.
 * </p>
 * <p>
 * You should later free this object using @ref AB_Banking_free.
 * </p>
 * <p>
 * This function does not actually load the configuration file or setup
 * AqBanking, that is performed by @ref AB_Banking_Init.
 * </p>
 *
 * @return new instance of AB_BANKING
 *
 * @param appName name of the application which wants to use AqBanking.
 * This allows AqBanking to separate settings and data for multiple
 * applications.
 *
 * @param dname Path for the directory containing the user data of
 * AqBanking. You should in most cases present a NULL for this
 * parameter, which means AqBanking will choose the default user
 * data folder which is "$HOME/.aqbanking".
 * The configuration itself is handled using GWEN's GWEN_ConfigMgr
 * module (see @ref GWEN_ConfigMgr_Factory). That module stores the
 * configuration in AqBanking's subfolder "settings" (i.e. the
 * full path to the user/account configuration is "$HOME/.aqbanking/settings").
 *
 * @param extensions use 0 for now.
 */
AQBANKING_API
AB_BANKING *AB_Banking_new(const char *appName,
			   const char *dname,
			   uint32_t extensions);


/**
 * Destroys the given instance of AqBanking. Please note that if
 * @ref AB_Banking_Init has been called on this object then
 * @ref  AB_Banking_Fini should be called before this function.
 */
AQBANKING_API 
void AB_Banking_free(AB_BANKING *ab);


AQBANKING_API 
void AB_Banking_GetVersion(int *major,
			   int *minor,
			   int *patchlevel,
			   int *build);


/**
 * Initializes AqBanking.
 * This sets up the plugins, plugin managers and path managers.
 *
 * @return 0 if ok, error code otherwise (see @ref AB_ERROR)
 *
 * @param ab banking interface
 */
AQBANKING_API 
int AB_Banking_Init(AB_BANKING *ab);

/**
 * Deinitializes AqBanking thus allowing it to save its data and to unload
 * backends.
 *
 * @return 0 if ok, error code otherwise (see @ref AB_ERROR)
 *
 * @param ab banking interface
 */
AQBANKING_API 
int AB_Banking_Fini(AB_BANKING *ab);


/*@}*/



/** @name Application Information, Shared Data
 *
 */
/*@{*/
/**
 * Returns the application name as given to @ref AB_Banking_new.
 * @param ab pointer to the AB_BANKING object
 */
AQBANKING_API 
const char *AB_Banking_GetAppName(const AB_BANKING *ab);

/**
 * Returns the escaped version of the application name. This name can
 * safely be used to create file paths since all special characters (like
 * '/', '.' etc) are escaped.
 * @param ab pointer to the AB_BANKING object
 */
AQBANKING_API 
const char *AB_Banking_GetEscapedAppName(const AB_BANKING *ab);

/**
 * Returns the name of the user folder for AqBanking's data.
 * Normally this is something like "/home/me/.aqbanking".
 * @return 0 if ok, error code otherwise (see @ref AB_ERROR)
 * @param ab pointer to the AB_BANKING object
 * @param buf GWEN_BUFFER to append the path name to
 */
AQBANKING_API 
int AB_Banking_GetUserDataDir(const AB_BANKING *ab, GWEN_BUFFER *buf);


/**
 * Returns the path to a folder to which shared data can be stored.
 * This might be used by multiple applications if they wish to share some
 * of their data, e.g. QBankManager and AqMoney3 share their transaction
 * storage so that both may work with it.
 * Please note that this folder does not necessarily exist, but you are free
 * to create it.
 */
AQBANKING_API 
int AB_Banking_GetSharedDataDir(const AB_BANKING *ab,
                                const char *name,
                                GWEN_BUFFER *buf);


/**
 * Load the given configuration subgroup from the shared application data database.
 *
 * This configuration database is used to store dialog sizes and other things that might
 * be shared among applications using AqBanking.
 *
 * @return 0 on success, error code otherwise
 * @param ab pointer to the AB_BANKING object
 * @param name name of the configuration subgroup
 * @param pDb pointer to a variable to receive the configuration loaded
 */
AQBANKING_API
int AB_Banking_LoadSharedConfig(AB_BANKING *ab, const char *name, GWEN_DB_NODE **pDb);


/**
 * Save the given configuration subgroup to the shared application data database.
 *
 * This configuration database is used to store dialog sizes and other things that might
 * be shared among applications using AqBanking.
 *
 * @return 0 on success, error code otherwise
 * @param ab pointer to the AB_BANKING object
 * @param name name of the configuration subgroup
 * @param db pointer to the configuration to store
 */
AQBANKING_API
int AB_Banking_SaveSharedConfig(AB_BANKING *ab, const char *name, GWEN_DB_NODE *db);


/**
 * Exclusively lock the given configuration subgroup of the shared application data database.
 *
 * This configuration database is used to store dialog sizes and other things that might
 * be shared among applications using AqBanking.
 *
 * @return 0 on success, error code otherwise
 * @param ab pointer to the AB_BANKING object
 * @param name name of the configuration subgroup
 */
AQBANKING_API
int AB_Banking_LockSharedConfig(AB_BANKING *ab, const char *name);


/**
 * Unlock the given configuration subgroup of the shared application data database.
 *
 * This configuration database is used to store dialog sizes and other things that might
 * be shared among applications using AqBanking.
 *
 * @return 0 on success, error code otherwise
 * @param ab pointer to the AB_BANKING object
 * @param name name of the configuration subgroup
 */
AQBANKING_API
int AB_Banking_UnlockSharedConfig(AB_BANKING *ab, const char *name);

/*@}*/



/** @name Working With Online Service Providers
 *
 * Applications may let AqBanking store global application specific data.
 */
/*@{*/

/**
 * Create a list of available online banking providers.
 */
AQBANKING_API GWEN_PLUGIN_DESCRIPTION_LIST2 *AB_Banking_GetProviderDescrs(AB_BANKING *ab);


/**
 * Load the given backend (if necessary), call the control function with the given arguments and unload the backend.
 */
AQBANKING_API int AB_Banking_ProviderControl(AB_BANKING *ab, const char *backendName, int argc, char **argv);


/**
 * Create a dialog which allows to create a new user.
 *
 * @return dialog
 *
 * @param ab pointer to the AqBanking object
 *
 * @param backend name of the backend for which a user is to be created
 *   (e.g. "aqhbci", "aqebics" etc)
 *
 * @param mode additional parameter depending on the backend. it can be used
 *   to specify the user type to be created (e.g. for HBCI those values
 *   specify whether PIN/TAN, keyfile or chipcard users are to be created,
 *   see @ref AqHBCI_NewUserDialog_CodeGeneric and following).
 *   Use value 0 for the generic dialog.
 */
AQBANKING_API 
GWEN_DIALOG *AB_Banking_GetNewUserDialog(AB_BANKING *ab,
					 const char *backend,
					 int mode);

/*@}*/




/*@}*/ /* addtogroup */


#ifdef __cplusplus
}
#endif



#endif



