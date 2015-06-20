#ifndef _TRKTOOL2_H_
#define _TRKTOOL2_H_ 1

#include "trktool.h"

#ifdef __cplusplus
extern "C" {
#endif

const TRK_RECORD_TYPE TRK_SCR_TYPE = 1;

typedef TRK_UINT		TRK_VID;

enum _TrkSysVid {
	VID_InternalId = 10000,
	VID_Id = 10001,
	VID_Title = 10002,
	VID_Description = 10003,
	VID_Submitter = 10004,
	VID_SubmitDate = 10005,
	VID_Owner = 10006,
	VID_State = 10007,
	VID_CloseDate = 10008,
	VID_FirstTransactionId = 10010,
	VID_LastTransactionId = 10011,
	VID_LockUserId = 10012,
	VID_TypeId = 10013
};

TRKAPI TrkGetRecordTypeFromVid( //@12
	TRK_HANDLE					trkHandle,				// Input
	TRK_VID						trkVid,					// Input
	TRK_RECORD_TYPE FAR*		pRecordType);			// Output
	
TRKAPI TrkGetFieldNameFromVid( //@20 
	TRK_HANDLE					trkHandle,				// Input
	TRK_RECORD_TYPE				recordType,				// Input
	TRK_VID						trkVid,					// Input
	TRK_UINT					bufferSize,				// Input
	TRK_STR						fieldName);				// Output

TRKAPI TrkGetFieldVid( //@16 
	TRK_HANDLE					trkHandle,				// Input
	TRK_CSTR					fieldName,				// Input
	TRK_RECORD_TYPE				recordType,				// Input
	TRK_VID FAR*				trkVid);				// Output

TRKAPI TrkGetRecordIsLocked( //@16 
	TRK_HANDLE					trkHandle,				// Input
	TRK_RECORD_TYPE				recordType,				// Input
	TRK_UINT					recordId,				// Input
	TRK_UINT FAR*				lockUser);				// Output

TRKAPI TrkGetFieldSubmitRights( //@16
	TRK_HANDLE					trkHandle,				// Input
	TRK_CSTR					fieldName,				// Input
	TRK_RECORD_TYPE				recordType,				// Input
	TRK_UINT FAR*				rights);				// Output

TRKAPI TrkGetFieldUpdateRights( //@16
    TRK_HANDLE					trkHandle,				// Input
    TRK_CSTR					fieldName,				// Input
	TRK_RECORD_TYPE				recordType,				// Input
	TRK_UINT FAR*				rights);				// Output

TRKAPI TrkGetFieldIsNullValid( //@16
    TRK_HANDLE					trkHandle,				// Input
    TRK_CSTR					fieldName,				// Input
    TRK_RECORD_TYPE				recordType,				// Input
    TRK_BOOL FAR*				enabled);				// Output


TRKAPI TrkGetDescriptionName( //@12
	TRK_HANDLE					trkHandle,				// Input
	//TRK_RECORD_TYPE				recordType,				// Input
	TRK_UINT					bufferSize,				// Input
	TRK_STR						fieldName);				// Output

TRKAPI TrkGetQueryIsPublic( //@12
                            TRK_HANDLE					trkHandle,				// Input
                            TRK_CSTR					queryName,				// Input
                            TRK_UINT FAR*				isPublic);				// Output


TRKAPI TrkInitNoteTitleList( //@4
                             TRK_NOTE_HANDLE					trkHandle				// Input
                             );
TRKAPI TrkGetNextNoteTitle( //@16
                            TRK_NOTE_HANDLE					trkHandle,				// Input
                            TRK_UINT					bufferSize,				// Input
                            TRK_STR						noteTitle,				// Output
                            TRK_UINT FAR*				unknown);				// Output

TRKAPI TrkGetNotePermissions( //@8
                              TRK_NOTE_HANDLE				trkNoteHandle,
                              TRK_UINT FAR*     rights);			// Input


TRKAPI TrkGetAttachedFileInfo( //@24
                               TRK_ATTFILE_HANDLE			trkAttFileHandle,		// Input
                               TRK_STR  buf1,
                               TRK_STR  buf2,
                               TRK_STR  buf3,
                               TRK_STR  buf4,
                               TRK_STR  buf5
                               /*TRK_UINT             		bufferSize,				// Input
                               TRK_STR						userName,
                               TRK_UINT FAR*				filesize,				// Output
                               TRK_TIME FAR*				pTimestamp,			// Output
                               TRK_FILE_STORAGE_MODE FAR*	pStorageMode			// Output
                                                              */
                               );


TRKAPI TrkInitChangeList( //@4
                          TRK_RECORD_HANDLE			trkRecordHandle);				// Input
TRKAPI TrkGetNextChange( //@12
                         TRK_RECORD_HANDLE			trkRecordHandle,				// Input
                         TRK_UINT					bufferSize,				// Input
                         TRK_STR					changes);				// Output


TRKAPI TrkGetNextChoiceInternal( //@24
                                 TRK_HANDLE	trkHandle,				// Input
                                 TRK_UINT	bufferSize,				// Input
                                 TRK_STR	label,			// Output
                                 TRK_UINT	FAR *id,				// Input
                                 TRK_UINT	FAR *order,				// Input
                                 TRK_UINT	FAR *weight				// Input
                                 );

TRKAPI TrkGetNextUserInternal( //@20
                               TRK_HANDLE					trkHandle,				// Input
                               TRK_UINT					bufferSize,				// Input
                               TRK_STR						userName,				// Output
                               TRK_UINT	FAR *id,				// Input
                               TRK_UINT	FAR *order				// Input
);

/*
qryId
 int
 Internal unique ID for this query. [PK]

qryClass
 int
 A value of 0 signifies a query that was created in version v6.5 or earlier of Tracker. Values of 1 and greater signify queries created in later versions.

qryFlags
 int
 Reserved for future use.

qryTypeId
 int
 The record type ID from the trktype table that this query corresponds to. Foreign key: typeVid in the trktype table.

qryName
 char(64)
 The name of the query. If it is empty, then it is an embedded query from a report.

qryAuthId
 int
 The user ID that originally authored this query. Foreign key: usrId in the trkusr table.

qryOwnerId
 int
 The user ID that currently owns this query. Stored separately from author ID for permission reasons and in case the original author is deleted. The initial owner is the author. Foreign key: usrId in the trkusr table.

qryCrDate
 int
 The date and time this report was created. Tracker does not display this value anywhere.

qryMdDate
 int
 The date and time this report was last modified. Tracker does not display this value anywhere.

qryTitle
 varchar(128)
 The name of the query; a duplicate of the qryname field. If it is empty, then it is an embedded query from a report.

qryCont
 text
 A null terminated, comma-separated string representing the contents of the query. See Notes below.

qryComment
 varchar(254)
 A comment about the query. Tracker displays it in the Query Run dialog.

qryDescr
 varchar(254)
 Reserved for future use.

qryParm1 - qryParm8
 int
 Reserved for future use.

*/
TRKAPI TrkSaveQuery( //@24
                     TRK_HANDLE	trkHandle,				// Input
                     TRK_CSTR					queryName,				// Input
                     TRK_CSTR					queryTitle,				// Input
//                     TRK_UINT userId,
                     TRK_CSTR queryCont,
                     TRK_CSTR queryDescr,
                     TRK_RECORD_TYPE recordType
                     );
//_TrkSaveUserQuery@12


//_TrkHandleAlloc@8
//_TrkHandleFree@4
//_TrkProjectLogin@36
//_TrkProjectLoginEx@20
//_TrkProjectLogout@4
//_TrkSetIniFile@8
//_TrkGetIniFile@12
//_TrkSetNumericAttribute@12
//_TrkGetNumericAttribute@12
//_TrkInitProjectList@24
//_TrkGetNextProject@12
//_TrkInitDBMSTypeList@4
//_TrkGetNextDBMSType@12
//_TrkGetLoginUserName@12
//_TrkGetLoginProjectName@12
//_TrkGetLoginDBMSType@12
//_TrkGetLoginDBMSName@12
//_TrkInitRecordTypeList@4
//_TrkGetNextRecordType@8
//_TrkGetRecordTypeName@16
//_TrkInitFieldList@8
//_TrkGetNextField@16
//_TrkGetFieldType@16
//_TrkGetFieldMaxDataLength@16
//_TrkGetFieldDefaultStringValue@20
//_TrkGetFieldDefaultNumericValue@16
//_TrkGetFieldRange@20
//_TrkInitQueryNameList@4
//_TrkGetNextQueryName@16
//_TrkGetQueryRecordType@12
//_TrkInitUserList@4        +
//_TrkGetNextUser@12        +
//_TrkGetUserFullName@16    +
//_TrkGetUserEmail@16       +
//_TrkInitChoiceList@12
//_TrkGetNextChoice@12
//_TrkGetProjectTransactionID@12
//_TrkRecordHandleAlloc@8
//_TrkRecordHandleFree@4
//_TrkQueryInitRecordList@16
//_TrkInTrayInitRecordList@16
//_TrkGetNextRecord@4
//_TrkGetSingleRecord@12
//_TrkNewRecordBegin@8
//_TrkNewRecordCommit@8
//_TrkRecordCancelTransaction@4
//_TrkUpdateRecordBegin@4
//_TrkUpdateRecordCommit@8
//_TrkDeleteRecord@4
//_TrkGetRecordTransactionID@12
//_TrkGetRecordRecordType@8
//_TrkGetNumericFieldValue@12
//_TrkGetStringFieldValue@16
//_TrkSetNumericFieldValue@12
//_TrkSetStringFieldValue@12
//_TrkGetFieldTransactionID@12
//_TrkGetFieldAccessRights@12
//_TrkGetDescriptionDataLength@8
//_TrkGetDescriptionData@16
//_TrkSetDescriptionData@16
//_TrkGetDescriptionAccessRights@8
//_TrkGetDescriptionTransactionID@8
//_TrkNoteHandleAlloc@8
//_TrkNoteHandleFree@4
//_TrkInitNoteList@4
//_TrkGetNextNote@4
//_TrkAddNewNote@4
//_TrkDeleteNote@4
//_TrkGetNoteTitle@12
//_TrkSetNoteTitle@8
//_TrkGetNoteAuthor@12
//_TrkSetNoteAuthor@8
//_TrkGetNoteCreateTime@8
//_TrkSetNoteCreateTime@8
//_TrkGetNoteModifyTime@8
//_TrkSetNoteModifyTime@8
//_TrkGetNoteDataLength@8
//_TrkGetNoteData@16
//_TrkSetNoteData@16
//_TrkGetNoteTransactionID@8
//_TrkGetNoteMaxDataLength@8
//_TrkGetNoteMaxTitleLength@8
//_TrkAttachedFileHandleAlloc@8
//_TrkAttachedFileHandleFree@4
//_TrkInitAttachedFileList@4
//_TrkGetNextAttachedFile@4
//_TrkAddNewAttachedFile@12
//_TrkDeleteAttachedFile@4
//_TrkGetAttachedFileName@12
//_TrkSetAttachedFileName@8
//_TrkGetAttachedFileTime@8
//_TrkGetAttachedFileStorageMode@8
//_TrkExtractAttachedFile@8
//_TrkGetAttachedFileTransactionID@8
//_TrkAssociationHandleAlloc@8
//_TrkAssociationHandleFree@4
//_TrkInitAssociationList@4
//_TrkGetNextAssociation@4
//_TrkAddNewAssociation@4
//_TrkDeleteAssociation@4
//_TrkGetAssociationModuleName@12
//_TrkSetAssociationModuleName@8
//_TrkGetAssociationUser@12
//_TrkSetAssociationUser@8
//_TrkGetAssociationRevisionFound@12
//_TrkSetAssociationRevisionFound@8
//_TrkGetAssociationRevisionFixed@12
//_TrkSetAssociationRevisionFixed@8
//_TrkGetAssociationTimeFound@8
//_TrkSetAssociationTimeFound@8
//_TrkGetAssociationTimeFixed@8
//_TrkSetAssociationTimeFixed@8
//_TrkGetAssociationTextLength@8
//_TrkGetAssociationText@16
//_TrkSetAssociationText@16
//_TrkGetAssociationTransactionID@8
//_TrkRegisterCallback@16
//_TrkUnregisterCallback@8
//_TrkLoginInternal@12
//_TrkLogoutInternal@4
//_TrkProjectLoginDialog@76
//_TrkSetPassword@8
//_TrkProjectLoginDialogEx@56
//_TrkRegisterWindowHandle@8
//_TrkInvokeCallback@24
//_TrkGetDcsSession@8
//_TrkGetDcsProcess@8
//_TrkGetRecordTypeFromVid@12
//_TrkGetFieldNameFromVid@20
//_TrkGetFieldVid@16
//_TrkGetFieldIsNullValid@16
//_TrkGetFieldDefaultGroupId@16
//_TrkGetFieldRawData@36
//_TrkInitFormNameList@4
//_TrkGetNextFormName@24
//_TrkGetFormEntryInternal@36
//_TrkCustomQueryInitRecordList@16
//_TrkSaveQuery@24
//_TrkInitMacroList@4
//_TrkGetNextMacro@20
//_TrkGetMacroInfo@20
//_TrkGetMacroValue@24
//_TrkExpandMacro@20
//_TrkUserIsMemberOfGroup@16
//_TrkUserIsAdmin@12
//_TrkGetNextChoiceInternal@24
//_TrkInitGroupUserList@12
//_TrkGetNextGroupUser@12
//_TrkModuleInitRecordList@24
//_TrkGetRecordInternalID@8
//_TrkGetSingleRecordByInternalId@12
//_TrkGetDescriptionName@12
//_TrkGetFieldSubmitRights@16
//_TrkGetFieldUpdateRights@16
//_TrkGetBuildQueryPermission@4
//_TrkInitNoteTitleList@4
//_TrkGetNextNoteTitle@16
//_TrkClearInTrayRecords@4
//_TrkClearInTrayRecord@4
//_TrkGetRecordIsLocked@16
//_TrkGetRecordVersionLabel@12
//_TrkInitChangeList@4   +
//_TrkGetNextChange@12   +
//_TrkGetNextChangeInternal@24
//_TrkRevisionHandleAlloc@8
//_TrkRevisionHandleFree@4
//_TrkModuleInitRevisionList@16
//_TrkGetNextRevision@4
//_TrkGetRevisionModuleName@12
//_TrkGetRevisionUser@12
//_TrkGetRevisionFound@12
//_TrkGetRevisionFixed@12
//_TrkGetRevisionTimeFound@8
//_TrkGetRevisionTimeFixed@8
//_TrkGetRevisionTextLength@8
//_TrkGetRevisionText@16
//_TrkGetRevisionInternalRecordID@8
//_TrkGetRevisionTransactionID@8
//_TrkGetDateTimeDisplayString@16
//_TrkGetTimeDisplayString@16
//_TrkGetUserIsDeleted@12
//_TrkUpdateRecordBeginEx@8
//_TrkGetAttachedFileInfo@24
//_TrkGetAttachedFilePermissions@12
//_TrkGetNoteInfo@28
//_TrkGetNotePermissions@8
//_TrkGetAssociationInfo@28
//_TrkGetAssociationPermissions@8
//_TrkGetAssociationMaxDataLength@20
//_TrkProjectRefresh
//_TrkHtmlStyleSheetHandleAlloc@8
//_TrkHtmlStyleSheetHandleFree@4
//_TrkHtmlSetStyleSheet@12
//_TrkHtmlSetStyleSheetFromTemp@8
//_TrkHtmlSetStyleSheetFromQuery@8
//_TrkHtmlStyleSheetGenerate@24
//_TrkHtmlStyleSheetGenerateEx@24
//_TrkHtmlFormHandleAlloc@8
//_TrkHtmlFormHandleFree@4
//_TrkHtmlFormSetFormRecord@12
//_TrkHtmlFormSetStyleSheet@12
//_TrkHtmlFormInitFieldList@4
//_TrkHtmlFormSetDefaultFieldValue@12
//_TrkHtmlFormSetFieldValue@16
//_TrkHtmlFormGenerate@28
//_TrkHtmlFormGenerateEx@28
//_TrkValidateHandle@4
//_TrkGetWindowHandle@8
//_TrkInitProjectListEx@8
//_TrkGetNextFieldAndIds@28
//_TrkGetFieldTypeFromId@12
//_TrkGetFieldRawDataFromId@32
//_TrkInitStateList@4
//_TrkGetNextState@16
//_TrkGetStateFlagsFromId@16
//_TrkGetStateFlags@16
//_TrkGetStateNameFromId@16
//_TrkGetStateFormName@24
//_TrkGetRecordFormName@20
//_TrkStateTransitionIsAllowed@16
//_TrkGetQueryFieldFromVid@20
//_TrkGetUserQueryFieldFromVid@16
//_TrkGetQueryIsPublic@12
//_TrkGetUserQueryIsPublic@8
//_TrkSaveUserQuery@12
//_TrkDeleteQuery@8
//_TrkGetNextUserInternal@20
//_TrkInitGroupList@4
//_TrkGetNextGroup@20
//_TrkInitChoiceListFromFldId@8
//_TrkQueryByIDInitRecordList@16
//_TrkVerifyQuery@16
//_TrkVerifyQueryByID@16



//_TrkSetQuerySortVid@12
//_TrkGetRecordUpdatePermission@8
//_TrkRecordLock@12
//_TrkGetTimeDisplayInt@12
//_TrkInTrayGetRecordCount@8
//_TrkRuleHandleAlloc@8
//_TrkRuleHandleFree@4
//_TrkInitRuleListInternal@12
//_TrkGetNextRuleInternal@28
//_TrkGetRuleConditionType1@24
//_TrkGetRuleActionType1@20
//_TrkApplyRuleInternal@20
//_TrkTransitionHandleAlloc@8
//_TrkTransitionHandleFree@4
//_TrkInitTransitionList@4
//_TrkGetNextTransitionInternal@20
//_TrkGetTransitionEntry@28
//_TrkGetAdmRefreshId@8
//_TrkHtmlFormClearFieldValues@8
//_TrkHtmlGetFieldName@16
//_TrkHtmlFormGetHelpUrl@12
//_TrkHtmlFormGetLastError@12
//_TrkGetTempFileName@12
//_TrkGetTrackerBinDir@8
//_TrkAddNewAssociation@4
//_TrkAddNewAttachedFile@12
//_TrkAddNewNote@4
//_TrkApplyRuleInternal@20
//_TrkAssociationHandleAlloc@8
//_TrkAssociationHandleFree@4
//_TrkAttachedFileHandleAlloc@8
//_TrkAttachedFileHandleFree@4
//_TrkClearInTrayRecord@4
//_TrkClearInTrayRecords@4
//_TrkCustomQueryInitRecordList@16
//_TrkDeleteAssociation@4
//_TrkDeleteAttachedFile@4
//_TrkDeleteNote@4
//_TrkDeleteQuery@8
//_TrkDeleteRecord@4
//_TrkExpandMacro@20
//_TrkExtractAttachedFile@8
//_TrkGetAdmRefreshId@8
//_TrkGetAssociationInfo@28
//_TrkGetAssociationMaxDataLength@20
//_TrkGetAssociationModuleName@12
//_TrkGetAssociationPermissions@8
//_TrkGetAssociationRevisionFixed@12
//_TrkGetAssociationRevisionFound@12
//_TrkGetAssociationText@16
//_TrkGetAssociationTextLength@8
//_TrkGetAssociationTimeFixed@8
//_TrkGetAssociationTimeFound@8
//_TrkGetAssociationTransactionID@8
//_TrkGetAssociationUser@12
//_TrkGetAttachedFileInfo@24
//_TrkGetAttachedFileName@12
//_TrkGetAttachedFilePermissions@12
//_TrkGetAttachedFileStorageMode@8
//_TrkGetAttachedFileTime@8
//_TrkGetAttachedFileTransactionID@8
//_TrkGetBuildQueryPermission@4
//_TrkGetDateTimeDisplayString@16
//_TrkGetDcsProcess@8
//_TrkGetDcsSession@8
//_TrkGetDescriptionAccessRights@8
//_TrkGetDescriptionData@16
//_TrkGetDescriptionDataLength@8
//_TrkGetDescriptionName@12
//_TrkGetDescriptionTransactionID@8
//_TrkGetFieldAccessRights@12
//_TrkGetFieldDefaultGroupId@16
//_TrkGetFieldDefaultNumericValue@16
//_TrkGetFieldDefaultStringValue@20
//_TrkGetFieldIsNullValid@16
//_TrkGetFieldMaxDataLength@16
//_TrkGetFieldNameFromVid@20
//_TrkGetFieldRange@20
//_TrkGetFieldRawData@36
//_TrkGetFieldRawDataFromId@32
//_TrkGetFieldSubmitRights@16
//_TrkGetFieldTransactionID@12
//_TrkGetFieldType@16
//_TrkGetFieldTypeFromId@12
//_TrkGetFieldUpdateRights@16
//_TrkGetFieldVid@16
//_TrkGetFormEntryInternal@36
//_TrkGetIniFile@12
//_TrkGetLoginDBMSName@12
//_TrkGetLoginDBMSType@12
//_TrkGetLoginProjectName@12
//_TrkGetLoginUserName@12
//_TrkGetMacroInfo@20
//_TrkGetMacroValue@24
//_TrkGetNextAssociation@4
//_TrkGetNextAttachedFile@4
//_TrkGetNextChange@12
//_TrkGetNextChangeInternal@24
//_TrkGetNextChoice@12
//_TrkGetNextChoiceInternal@24
//_TrkGetNextDBMSType@12
//_TrkGetNextField@16
//_TrkGetNextFieldAndIds@28
//_TrkGetNextFormName@24
//_TrkGetNextGroup@20
//_TrkGetNextGroupUser@12
//_TrkGetNextMacro@20
//_TrkGetNextNote@4
//_TrkGetNextNoteTitle@16
//_TrkGetNextProject@12
//_TrkGetNextQueryName@16
//_TrkGetNextRecord@4
//_TrkGetNextRecordType@8
//_TrkGetNextRevision@4
//_TrkGetNextRuleInternal@28
//_TrkGetNextState@16
//_TrkGetNextTransitionInternal@20
//_TrkGetNextUser@12
//_TrkGetNextUserInternal@20
//_TrkGetNoteAuthor@12
//_TrkGetNoteCreateTime@8
//_TrkGetNoteData@16
//_TrkGetNoteDataLength@8
//_TrkGetNoteInfo@28
//_TrkGetNoteMaxDataLength@8
//_TrkGetNoteMaxTitleLength@8
//_TrkGetNoteModifyTime@8
//_TrkGetNotePermissions@8
//_TrkGetNoteTitle@12
//_TrkGetNoteTransactionID@8
//_TrkGetNumericAttribute@12
//_TrkGetNumericFieldValue@12
//_TrkGetProjectTransactionID@12
//_TrkGetQueryFieldFromVid@20
//_TrkGetQueryIsPublic@12
//_TrkGetQueryRecordType@12
//_TrkGetRecordFormName@20
//_TrkGetRecordInternalID@8
//_TrkGetRecordIsLocked@16
//_TrkGetRecordRecordType@8
//_TrkGetRecordTransactionID@12
//_TrkGetRecordTypeFromVid@12
//_TrkGetRecordTypeName@16
//_TrkGetRecordUpdatePermission@8
//_TrkGetRecordVersionLabel@12
//_TrkGetRevisionFixed@12
//_TrkGetRevisionFound@12
//_TrkGetRevisionInternalRecordID@8
//_TrkGetRevisionModuleName@12
//_TrkGetRevisionText@16
//_TrkGetRevisionTextLength@8
//_TrkGetRevisionTimeFixed@8
//_TrkGetRevisionTimeFound@8
//_TrkGetRevisionTransactionID@8
//_TrkGetRevisionUser@12
//_TrkGetRuleActionType1@20
//_TrkGetRuleConditionType1@24
//_TrkGetSingleRecord@12
//_TrkGetSingleRecordByInternalId@12
//_TrkGetStateFlags@16
//_TrkGetStateFlagsFromId@16
//_TrkGetStateFormName@24
//_TrkGetStateNameFromId@16
//_TrkGetStringFieldValue@16
//_TrkGetTempFileName@12
//_TrkGetTimeDisplayInt@12
//_TrkGetTimeDisplayString@16
//_TrkGetTrackerBinDir@8
//_TrkGetTransitionEntry@28
//_TrkGetUserEmail@16
//_TrkGetUserFullName@16
//_TrkGetUserIsDeleted@12
//_TrkGetUserQueryFieldFromVid@16
//_TrkGetUserQueryIsPublic@8
//_TrkGetWindowHandle@8
//_TrkHandleAlloc@8
//_TrkHandleFree@4
//_TrkHtmlFormClearFieldValues@8
//_TrkHtmlFormGenerate@28
//_TrkHtmlFormGenerateEx@28
//_TrkHtmlFormGetHelpUrl@12
//_TrkHtmlFormGetLastError@12
//_TrkHtmlFormHandleAlloc@8
//_TrkHtmlFormHandleFree@4
//_TrkHtmlFormInitFieldList@4
//_TrkHtmlFormSetDefaultFieldValue@12
//_TrkHtmlFormSetFieldValue@16
//_TrkHtmlFormSetFormRecord@12
//_TrkHtmlFormSetStyleSheet@12
//_TrkHtmlGetFieldName@16
//_TrkHtmlSetStyleSheet@12
//_TrkHtmlSetStyleSheetFromQuery@8
//_TrkHtmlSetStyleSheetFromTemp@8
//_TrkHtmlStyleSheetGenerate@24
//_TrkHtmlStyleSheetGenerateEx@24
//_TrkHtmlStyleSheetHandleAlloc@8
//_TrkHtmlStyleSheetHandleFree@4
//_TrkInTrayGetRecordCount@8
//_TrkInTrayInitRecordList@16
//_TrkInitAssociationList@4
//_TrkInitAttachedFileList@4
//_TrkInitChangeList@4
//_TrkInitChoiceList@12
//_TrkInitChoiceListFromFldId@8
//_TrkInitDBMSTypeList@4
//_TrkInitFieldList@8
//_TrkInitFormNameList@4
//_TrkInitGroupList@4
//_TrkInitGroupUserList@12
//_TrkInitMacroList@4
//_TrkInitNoteList@4
//_TrkInitNoteTitleList@4
//_TrkInitProjectList@24
//_TrkInitProjectListEx@8
//_TrkInitQueryNameList@4
//_TrkInitRecordTypeList@4
//_TrkInitRuleListInternal@12
//_TrkInitStateList@4
//_TrkInitTransitionList@4
//_TrkInitUserList@4
//_TrkInvokeCallback@24
//_TrkLoginInternal@12
//_TrkLogoutInternal@4
//_TrkModuleInitRecordList@24
//_TrkModuleInitRevisionList@16
//_TrkNewRecordBegin@8
//_TrkNewRecordCommit@8
//_TrkNoteHandleAlloc@8
//_TrkNoteHandleFree@4
//_TrkProjectLogin@36
//_TrkProjectLoginDialog@76
//_TrkProjectLoginDialogEx@56
//_TrkProjectLoginEx@20
//_TrkProjectLogout@4
//_TrkProjectRefresh
//_TrkQueryByIDInitRecordList@16
//_TrkQueryInitRecordList@16
//_TrkRecordCancelTransaction@4
//_TrkRecordHandleAlloc@8
//_TrkRecordHandleFree@4
//_TrkRecordLock@12
//_TrkRegisterCallback@16
//_TrkRegisterWindowHandle@8
//_TrkRevisionHandleAlloc@8
//_TrkRevisionHandleFree@4
//_TrkRuleHandleAlloc@8
//_TrkRuleHandleFree@4
//_TrkSaveQuery@24
//_TrkSaveUserQuery@12
//_TrkSetAssociationModuleName@8
//_TrkSetAssociationRevisionFixed@8
//_TrkSetAssociationRevisionFound@8
//_TrkSetAssociationText@16
//_TrkSetAssociationTimeFixed@8
//_TrkSetAssociationTimeFound@8
//_TrkSetAssociationUser@8
//_TrkSetAttachedFileName@8
//_TrkSetDescriptionData@16
//_TrkSetIniFile@8
//_TrkSetNoteAuthor@8
//_TrkSetNoteCreateTime@8
//_TrkSetNoteData@16
//_TrkSetNoteModifyTime@8
//_TrkSetNoteTitle@8
//_TrkSetNumericAttribute@12
//_TrkSetNumericFieldValue@12
//_TrkSetPassword@8
//_TrkSetQuerySortVid@12
//_TrkSetStringFieldValue@12
//_TrkStateTransitionIsAllowed@16
//_TrkTransitionHandleAlloc@8
//_TrkTransitionHandleFree@4
//_TrkUnregisterCallback@8
//_TrkUpdateRecordBegin@4
//_TrkUpdateRecordBeginEx@8
//_TrkUpdateRecordCommit@8
//_TrkUserIsAdmin@12
//_TrkUserIsMemberOfGroup@16
//_TrkValidateHandle@4
//_TrkVerifyQuery@16
//_TrkVerifyQueryByID@16


#ifdef __cplusplus
}
#endif

#endif
