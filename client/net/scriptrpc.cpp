#include "../main.h"

extern CNetGame*	pNetGame;
extern CChatWindow*	pChatWindow;
extern CDeathWindow	*pDeathWindow;
extern CGame * pGame;

//----------------------------------------------------

void ScrSetSpawnInfo(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	PLAYER_SPAWN_INFO SpawnInfo;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	bitStream->Read((PCHAR)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	pPlayerPool->GetLocalPlayer()->SetSpawnInfo(&SpawnInfo);
}

//----------------------------------------------------

void ScrSetPlayerTeam(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	BYTE byteTeam;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bitStream->Read(playerId);
	bitStream->Read(byteTeam);

	if (playerId == pPlayerPool->GetLocalPlayerID()) {
		pPlayerPool->GetLocalPlayer()->SetTeam(byteTeam);
	}
	else {
		CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
		if (pPlayer) pPlayer->SetTeam(byteTeam);
	}
}

//----------------------------------------------------

void ScrSetPlayerName(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	BYTE byteNickLen;
	char szNewName[MAX_PLAYER_NAME + 1];
	BYTE byteSuccess;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bitStream->Read(playerId);
	bitStream->Read(byteNickLen);

	if (byteNickLen > MAX_PLAYER_NAME) return;

	bitStream->Read(szNewName, byteNickLen);
	bitStream->Read(byteSuccess);

	szNewName[byteNickLen] = '\0';

	if (byteSuccess == 1) pPlayerPool->SetPlayerName(playerId, szNewName);

	// Extra addition which we need to do if this is the local player;
	if (pPlayerPool->GetLocalPlayerID() == playerId)
		pPlayerPool->SetLocalPlayerName(szNewName);
}

void ScrSetPlayerSkin(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	int iPlayerID;
	unsigned int uiSkin;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bitStream->Read(iPlayerID);
	bitStream->Read(uiSkin);

	if (iPlayerID == pPlayerPool->GetLocalPlayerID()) {
		pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetModelIndex(uiSkin);
	}
	else {
		if (pPlayerPool->GetSlotState(iPlayerID) && pPlayerPool->GetAt(iPlayerID)->GetPlayerPed()) {
			pPlayerPool->GetAt(iPlayerID)->GetPlayerPed()->SetModelIndex(uiSkin);
		}
	}
}

//----------------------------------------------------

void ScrSetPlayerPos(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	VECTOR vecPos;

	bitStream->Read(vecPos.X);
	bitStream->Read(vecPos.Y);
	bitStream->Read(vecPos.Z);

	pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.X, vecPos.Y, vecPos.Z);
}

//----------------------------------------------------

void ScrSetPlayerPosFindZ(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	VECTOR vecPos;

	bitStream->Read(vecPos.X);
	bitStream->Read(vecPos.Y);
	bitStream->Read(vecPos.Z);

	vecPos.Z = pGame->FindGroundZForCoord(vecPos.X, vecPos.Y, vecPos.Z);
	vecPos.Z += 1.5f;

	pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.X, vecPos.Y, vecPos.Z);
}

//----------------------------------------------------

void ScrSetPlayerHealth(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	float fHealth;

	bitStream->Read(fHealth);

	//pChatWindow->AddDebugMessage("Setting your health to: %f", fHealth);
	pLocalPlayer->GetPlayerPed()->SetHealth(fHealth);
}

//----------------------------------------------------

void ScrPutPlayerInVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE vehicleid;
	BYTE seatid;
	bitStream->Read(vehicleid);
	bitStream->Read(seatid);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	int iVehicleIndex = pNetGame->GetVehiclePool()->FindGtaIDFromID(vehicleid);
	CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehicleid);

	if (iVehicleIndex && pVehicle) {
		pGame->FindPlayerPed()->PutDirectlyInVehicle(iVehicleIndex, seatid);
	}
}

//----------------------------------------------------

void ScrRemovePlayerFromVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	pPlayerPool->GetLocalPlayer()->GetPlayerPed()->ExitCurrentVehicle();
}

//----------------------------------------------------

void ScrSetPlayerColor(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	SACMPLAYER playerId;
	DWORD dwColor;

	bitStream->Read(playerId);
	bitStream->Read(dwColor);

	if (playerId == pPlayerPool->GetLocalPlayerID()) {
		pPlayerPool->GetLocalPlayer()->SetPlayerColor(dwColor);
	}
	else {
		CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
		if (pPlayer)	pPlayer->SetPlayerColor(dwColor);
	}
}

//----------------------------------------------------

void ScrDisplayGameText(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	char szMessage[512];
	int iType;
	int iTime;
	int iLength;

	bitStream->Read(iType);
	bitStream->Read(iTime);
	bitStream->Read(iLength);

	if (iLength > 512) return;

	bitStream->Read(szMessage, iLength);
	szMessage[iLength] = '\0';

	pGame->DisplayGameText(szMessage, iTime, iType);
}

//----------------------------------------------------

void ScrSetInterior(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteInterior;
	bitStream->Read(byteInterior);

	//pChatWindow->AddDebugMessage("ScrSetInterior(%u)",byteInterior);

	pGame->FindPlayerPed()->SetInterior(byteInterior);
}

//----------------------------------------------------

void ScrSetCameraPos(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	VECTOR vecPos;
	bitStream->Read(vecPos.X);
	bitStream->Read(vecPos.Y);
	bitStream->Read(vecPos.Z);
	pGame->GetCamera()->SetPosition(vecPos.X, vecPos.Y, vecPos.Z, 0.0f, 0.0f, 0.0f);
}

//----------------------------------------------------

void ScrSetCameraLookAt(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	VECTOR vecPos;
	bitStream->Read(vecPos.X);
	bitStream->Read(vecPos.Y);
	bitStream->Read(vecPos.Z);
	pGame->GetCamera()->LookAtPoint(vecPos.X, vecPos.Y, vecPos.Z, 2);
}

//----------------------------------------------------

void ScrSetVehiclePos(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleId;
	float fX, fY, fZ;
	bitStream->Read(VehicleId);
	bitStream->Read(fX);
	bitStream->Read(fY);
	bitStream->Read(fZ);

	if (pNetGame && pNetGame->GetVehiclePool()) {
		if (pNetGame->GetVehiclePool()->GetSlotState(VehicleId)) {
			pNetGame->GetVehiclePool()->GetAt(VehicleId)->TeleportTo(fX, fY, fZ);
		}
	}
}

//----------------------------------------------------

void ScrSetVehicleZAngle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleId;
	float fZAngle;
	bitStream->Read(VehicleId);
	bitStream->Read(fZAngle);

	ScriptCommand(&set_car_z_angle, pNetGame->GetVehiclePool()->GetAt(VehicleId)->m_dwGTAId, fZAngle);
}

//----------------------------------------------------

void ScrVehicleParams(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleID;
	BYTE byteObjectiveVehicle;
	BYTE byteDoorsLocked;

	bitStream->Read(VehicleID);
	bitStream->Read(byteObjectiveVehicle);
	bitStream->Read(byteDoorsLocked);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	pVehiclePool->AssignSpecialParamsToVehicle(VehicleID, byteObjectiveVehicle, byteDoorsLocked);

}

//----------------------------------------------------

void ScrLinkVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleID;
	BYTE byteInterior;

	bitStream->Read(VehicleID);
	bitStream->Read(byteInterior);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	pVehiclePool->LinkToInterior(VehicleID, (int)byteInterior);
}

//----------------------------------------------------

void ScrSetCameraBehindPlayer(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	pGame->GetCamera()->SetBehindPlayer();
}

//----------------------------------------------------

void ScrTogglePlayerControllable(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteControllable;
	bitStream->Read(byteControllable);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable((int)byteControllable);
}

//----------------------------------------------------

void ScrPlaySound(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	int iSound;
	float fX, fY, fZ;
	bitStream->Read(iSound);
	bitStream->Read(fX);
	bitStream->Read(fY);
	bitStream->Read(fZ);

	pGame->PlaySound(iSound, fX, fY, fZ);
}

//----------------------------------------------------

void ScrSetWorldBounds(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	bitStream->Read(pNetGame->m_WorldBounds[0]);
	bitStream->Read(pNetGame->m_WorldBounds[1]);
	bitStream->Read(pNetGame->m_WorldBounds[2]);
	bitStream->Read(pNetGame->m_WorldBounds[3]);
}

//----------------------------------------------------

void ScrHaveSomeMoney(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	int iAmount;
	bitStream->Read(iAmount);
	pGame->AddToLocalMoney(iAmount);
}

//----------------------------------------------------

void ScrSetPlayerFacingAngle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	float fAngle;
	bitStream->Read(fAngle);
	pGame->FindPlayerPed()->ForceTargetRotation(fAngle);
}

//----------------------------------------------------

void ScrResetMoney(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	pGame->ResetLocalMoney();
}

//----------------------------------------------------

void ScrResetPlayerWeapons(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	pPlayerPed->ClearAllWeapons();
}

//----------------------------------------------------

void ScrGivePlayerWeapon(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	int iWeaponID;
	int iAmmo;

	bitStream->Read(iWeaponID);
	bitStream->Read(iAmmo);

	CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	pPlayerPed->GiveWeapon(iWeaponID, iAmmo);
}

//----------------------------------------------------

void ScrRespawnVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleID;
	bitStream->Read(VehicleID);
}

//----------------------------------------------------

void ScrDeathMessage(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER Killer, Killee;
	BYTE byteWeapon;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	PCHAR szKillerName = NULL;
	PCHAR szKilleeName = NULL;
	DWORD dwKillerColor = 0;
	DWORD dwKilleeColor = 0;

	bitStream->Read(Killer);
	bitStream->Read(Killee);
	bitStream->Read(byteWeapon);

	//pChatWindow->AddDebugMessage("RawDeath: %u %u %u",Killer,Killee,byteWeapon);

	if (Killee == INVALID_PLAYER_ID) return;

	// Determine the killer's name and color
	if (Killer == INVALID_PLAYER_ID) {
		szKillerName = NULL; dwKillerColor = 0;
	}
	else {
		if (pPlayerPool->GetLocalPlayerID() == Killer) {
			szKillerName = pPlayerPool->GetLocalPlayerName();
			dwKillerColor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
		}
		else {
			if (pPlayerPool->GetSlotState(Killer)) {
				szKillerName = pPlayerPool->GetPlayerName(Killer);
				dwKillerColor = pPlayerPool->GetAt(Killer)->GetPlayerColorAsARGB();
			}
			else {
				//pChatWindow->AddDebugMessage("Slot State Killer FALSE");
				szKillerName = NULL; dwKillerColor = 0;
			}
		}
	}

	// Determine the killee's name and color
	if (pPlayerPool->GetLocalPlayerID() == Killee) {
		szKilleeName = pPlayerPool->GetLocalPlayerName();
		dwKilleeColor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
	}
	else {
		if (pPlayerPool->GetSlotState(Killee)) {
			szKilleeName = pPlayerPool->GetPlayerName(Killee);
			dwKilleeColor = pPlayerPool->GetAt(Killee)->GetPlayerColorAsARGB();
		}
		else {
			//pChatWindow->AddDebugMessage("Slot State Killee FALSE");
			szKilleeName = NULL; dwKilleeColor = 0;
		}
	}

	if (pDeathWindow) pDeathWindow->AddMessage(szKillerName, szKilleeName, dwKillerColor, dwKilleeColor, byteWeapon);
}

//----------------------------------------------------

void ScrSetMapIcon(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteIndex;
	BYTE byteIcon;
	DWORD byteColor;
	float fPos[3];

	bitStream->Read(byteIndex);
	bitStream->Read(fPos[0]);
	bitStream->Read(fPos[1]);
	bitStream->Read(fPos[2]);
	bitStream->Read(byteIcon);
	bitStream->Read(byteColor);

	pNetGame->SetMapIcon(byteIndex, fPos[0], fPos[1], fPos[2], byteIcon, byteColor);
}

void ScrDisableMapIcon(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteIndex;

	bitStream->Read(byteIndex);

	pNetGame->DisableMapIcon(byteIndex);
}

void ScrSetPlayerArmour(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	float fArmour;
	bitStream->Read(fArmour);

	pLocalPlayer->GetPlayerPed()->SetArmour(fArmour);
}

void ScrSetWeaponAmmo(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	BYTE byteWeapon;
	WORD wordAmmo;

	bitStream->Read(byteWeapon);
	bitStream->Read(wordAmmo);

	pLocalPlayer->GetPlayerPed()->SetAmmo(byteWeapon, wordAmmo);
}

//----------------------------------------------------

void ScrSetGravity(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	float fGravity;
	bitStream->Read(fGravity);
	pGame->SetGravity(fGravity);
}

//----------------------------------------------------

void ScrSetVehicleHealth(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	float fVehicleHealth;
	SACMVEHICLE VehicleID;

	bitStream->Read(VehicleID);
	bitStream->Read(fVehicleHealth);

	if (pNetGame->GetVehiclePool()->GetSlotState(VehicleID))
	{
		CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(VehicleID);
		pVehicle->SetHealth(fVehicleHealth);
		pVehicle->SetReportedHealth(fVehicleHealth);
	}
}

//----------------------------------------------------

void ScrAttachTrailerToVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE TrailerID, VehicleID;
	bitStream->Read(TrailerID);
	bitStream->Read(VehicleID);
	CVehicle* pTrailer = pNetGame->GetVehiclePool()->GetAt(TrailerID);
	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(VehicleID);

	pVehicle->SetTrailer(pTrailer);
	pVehicle->AttachTrailer();
}

//----------------------------------------------------

void ScrDetachTrailerFromVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleID;
	bitStream->Read(VehicleID);
	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(VehicleID);

	pVehicle->DetachTrailer();
	pVehicle->SetTrailer(NULL);
}

//----------------------------------------------------

void ScrCreateObject(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	byte byteObjectID;
	int iModel;
	VECTOR vecPos, vecRot;
	bitStream->Read(byteObjectID);
	bitStream->Read(iModel);

	bitStream->Read(vecPos.X);
	bitStream->Read(vecPos.Y);
	bitStream->Read(vecPos.Z);

	bitStream->Read(vecRot.X);
	bitStream->Read(vecRot.Y);
	bitStream->Read(vecRot.Z);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	pObjectPool->New(byteObjectID, iModel, vecPos, vecRot);
}

//----------------------------------------------------

void ScrSetObjectPos(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	byte byteObjectID;
	float fX, fY, fZ, fRotation;
	bitStream->Read(byteObjectID);
	bitStream->Read(fX);
	bitStream->Read(fY);
	bitStream->Read(fZ);
	bitStream->Read(fRotation);

	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	CObject*		pObject = pObjectPool->GetAt(byteObjectID);
	if (pObject)
	{
		pObject->TeleportTo(fX, fY, fZ);
	}
}

//----------------------------------------------------

void ScrSetObjectRotation(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	byte byteObjectID;
	float fX, fY, fZ;
	bitStream->Read(byteObjectID);
	bitStream->Read(fX);
	bitStream->Read(fY);
	bitStream->Read(fZ);

	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	CObject*		pObject = pObjectPool->GetAt(byteObjectID);
	if (pObject)
	{
		pObject->InstantRotate(fX, fY, fZ);
	}
}

//----------------------------------------------------

void ScrDestroyObject(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	byte byteObjectID;
	bitStream->Read(byteObjectID);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (pObjectPool->GetAt(byteObjectID))
	{
		pObjectPool->Delete(byteObjectID);
	}
}

void ScrCreateExplosion(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	float X, Y, Z, Radius;
	int   iType;

	bitStream->Read(X);
	bitStream->Read(Y);
	bitStream->Read(Z);
	bitStream->Read(iType);
	bitStream->Read(Radius);

	ScriptCommand(&create_explosion_with_radius, X, Y, Z, iType, Radius);
}

void ScrShowNameTag(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	BYTE byteShow;

	bitStream->Read(playerId);
	bitStream->Read(byteShow);

	if (pNetGame->GetPlayerPool()->GetSlotState(playerId))
	{
		pNetGame->GetPlayerPool()->GetAt(playerId)->ShowNameTag(byteShow);
	}
}

void ScrMoveObject(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteObjectID;
	float curx, cury, curz, newx, newy, newz, speed;

	bitStream->Read(byteObjectID);
	bitStream->Read(curx);
	bitStream->Read(cury);
	bitStream->Read(curz);
	bitStream->Read(newx);
	bitStream->Read(newy);
	bitStream->Read(newz);
	bitStream->Read(speed);

	CObject* pObject = pNetGame->GetObjectPool()->GetAt(byteObjectID);
	if (pObject)
	{
		pObject->TeleportTo(curx, cury, curz);
		pObject->MoveTo(newx, newy, newz, speed);
	}
}

void ScrStopObject(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteObjectID;
	float newx, newy, newz;

	bitStream->Read(byteObjectID);
	bitStream->Read(newx);
	bitStream->Read(newy);
	bitStream->Read(newz);

	CObject* pObject = pNetGame->GetObjectPool()->GetAt(byteObjectID);
	if (pObject)
	{
		pObject->MoveTo(newx, newy, newz, pObject->m_fMoveSpeed);
	}
}

void ScrNumberPlate(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE Vehicle;
	CHAR cNumberPlate[9];

	bitStream->Read(Vehicle);
	bitStream->Read(cNumberPlate, 9);
}

//----------------------------------------------------

void ScrTogglePlayerSpectating(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	BOOL bToggle;
	bitStream->Read(bToggle);
	pPlayerPool->GetLocalPlayer()->ToggleSpectating(bToggle);
}

void ScrSetPlayerSpectating(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	bitStream->Read(playerId);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool->GetSlotState(playerId)) {
		pPlayerPool->GetAt(playerId)->SetState(PLAYER_STATE_SPECTATING);
	}
}

#define SPECTATE_TYPE_NORMAL	1
#define SPECTATE_TYPE_FIXED		2
#define SPECTATE_TYPE_SIDE		3

void ScrPlayerSpectatePlayer(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	BYTE byteMode;

	bitStream->Read(playerId);
	bitStream->Read(byteMode);

	switch (byteMode) {
	case SPECTATE_TYPE_FIXED:
		byteMode = 15;
		break;
	case SPECTATE_TYPE_SIDE:
		byteMode = 14;
		break;
	default:
		byteMode = 4;
	}
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectatePlayer(playerId);
}

void ScrPlayerSpectateVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleID;
	BYTE byteMode;

	bitStream->Read(VehicleID);
	bitStream->Read(byteMode);

	switch (byteMode) {
	case SPECTATE_TYPE_FIXED:
		byteMode = 15;
		break;
	case SPECTATE_TYPE_SIDE:
		byteMode = 14;
		break;
	default:
		byteMode = 3;
	}
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectateVehicle(VehicleID);
}

void ScrRemoveComponent(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE VehicleID;
	DWORD dwComponent;

	bitStream->Read(VehicleID);
	bitStream->Read(dwComponent);

	int iVehicleID;
	//int iComponent;

	if (!pNetGame) return;

	iVehicleID = pNetGame->GetVehiclePool()->FindGtaIDFromID(VehicleID);
	if (iVehicleID) ScriptCommand(&remove_component, iVehicleID, (int)dwComponent);
}

void ScrForceSpawnSelection(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	pNetGame->GetPlayerPool()->GetLocalPlayer()->ReturnToClassSelection();
}

void ScrAttachObjectToPlayer(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteObjectID;
	SACMPLAYER playerId;

	float OffsetX, OffsetY, OffsetZ, rX, rY, rZ;

	bitStream->Read(byteObjectID);
	bitStream->Read(playerId);

	bitStream->Read(OffsetX);
	bitStream->Read(OffsetY);
	bitStream->Read(OffsetZ);

	bitStream->Read(rX);
	bitStream->Read(rY);
	bitStream->Read(rZ);

	try {

		CObject* pObject = pNetGame->GetObjectPool()->GetAt(byteObjectID);

		if (!pObject)
			return;

		if (playerId == pNetGame->GetPlayerPool()->GetLocalPlayerID())
		{
			CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
			ScriptCommand(&attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
				OffsetX,
				OffsetY,
				OffsetZ,
				rX,
				rY,
				rZ);
		}
		else {
			CRemotePlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt(playerId);

			if (!pPlayer)
				return;

			ScriptCommand(&attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
				OffsetX,
				OffsetY,
				OffsetZ,
				rX,
				rY,
				rZ);
		}

	}
	catch (...) {}
}

void ScrInitMenu(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	if (!pNetGame) return;
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();

	BYTE byteMenuID;
	BOOL bColumns; // 0 = 1, 1 = 2
	CHAR cText[MAX_MENU_LINE];
	float fX;
	float fY;
	float fCol1;
	float fCol2 = 0.0;
	MENU_INT MenuInteraction;

	bitStream->Read(byteMenuID);
	bitStream->Read(bColumns);
	bitStream->Read(cText, MAX_MENU_LINE);
	bitStream->Read(fX);
	bitStream->Read(fY);
	bitStream->Read(fCol1);
	if (bColumns) bitStream->Read(fCol2);
	bitStream->Read(MenuInteraction.bMenu);
	for (BYTE i = 0; i < MAX_MENU_ITEMS; i++)
	{
		bitStream->Read(MenuInteraction.bRow[i]);
	}

	CMenu* pMenu;

	if (pMenuPool->GetSlotState(byteMenuID))
	{
		pMenuPool->Delete(byteMenuID);
	}

	pMenu = pMenuPool->New(byteMenuID, cText, fX, fY, ((BYTE)bColumns) + 1, fCol1, fCol2, &MenuInteraction);

	if (!pMenu) return;

	BYTE byteColCount;
	bitStream->Read(cText, MAX_MENU_LINE);
	pMenu->SetColumnTitle(0, cText);

	bitStream->Read(byteColCount);
	for (BYTE i = 0; i < byteColCount; i++)
	{
		bitStream->Read(cText, MAX_MENU_LINE);
		pMenu->AddMenuItem(0, i, cText);
	}

	if (bColumns)
	{
		bitStream->Read(cText, MAX_MENU_LINE);
		pMenu->SetColumnTitle(1, cText);

		bitStream->Read(byteColCount);
		for (BYTE i = 0; i < byteColCount; i++)
		{
			bitStream->Read(cText, MAX_MENU_LINE);
			pMenu->AddMenuItem(1, i, cText);
		}
	}
}

void ScrShowMenu(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	if (!pNetGame) return;
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();

	BYTE byteMenuID;
	bitStream->Read(byteMenuID);
	pNetGame->GetMenuPool()->ShowMenu(byteMenuID);
}

void ScrHideMenu(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	if (!pNetGame) return;
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();

	BYTE byteMenuID;
	bitStream->Read(byteMenuID);
	pNetGame->GetMenuPool()->HideMenu(byteMenuID);
}

void ScrSetPlayerWantedLevel(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	if (!pGame) return;

	BYTE byteLevel;
	bitStream->Read(byteLevel);
	pGame->SetWantedLevel(byteLevel);
}

void ScrShowTextDraw(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool)
	{
		WORD wTextID;
		TEXT_DRAW_TRANSMIT TextDrawTransmit;
		CHAR cText[MAX_TEXT_DRAW_LINE];
		bitStream->Read(wTextID);
		bitStream->Read((PCHAR)&TextDrawTransmit, sizeof(TEXT_DRAW_TRANSMIT));
		bitStream->Read(cText, MAX_TEXT_DRAW_LINE);

		char szDebug[256];
		sprintf(szDebug, "New TextDraw: %u\n", wTextID);
		OutputDebugString(szDebug);

		pTextDrawPool->New(wTextID, &TextDrawTransmit, cText);
	}
}

void ScrHideTextDraw(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();

	if (pTextDrawPool) {
		WORD wTextID;
		bitStream->Read(wTextID);

		char szDebug[256];
		sprintf(szDebug, "Deleteing TextDraw: %u\n", wTextID);
		OutputDebugString(szDebug);

		pTextDrawPool->Delete(wTextID);
	}
}

void ScrEditTextDraw(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool)
	{
		WORD wTextID;
		CHAR cText[MAX_TEXT_DRAW_LINE];
		bitStream->Read(wTextID);
		bitStream->Read(cText, MAX_TEXT_DRAW_LINE);
		CTextDraw* pText = pTextDrawPool->GetAt(wTextID);
		if (pText) pText->SetText(cText);
	}
}

void ScrAddGangZone(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		WORD wZoneID;
		float minx, miny, maxx, maxy;
		DWORD dwColor;
		bitStream->Read(wZoneID);
		bitStream->Read(minx);
		bitStream->Read(miny);
		bitStream->Read(maxx);
		bitStream->Read(maxy);
		bitStream->Read(dwColor);
		pGangZonePool->New(wZoneID, minx, miny, maxx, maxy, dwColor);
	}
}

void ScrRemoveGangZone(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		WORD wZoneID;
		bitStream->Read(wZoneID);
		pGangZonePool->Delete(wZoneID);
	}
}

void ScrFlashGangZone(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		WORD wZoneID;
		DWORD dwColor;
		bitStream->Read(wZoneID);
		bitStream->Read(dwColor);
		pGangZonePool->Flash(wZoneID, dwColor);
	}
}

void ScrStopFlashGangZone(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		WORD wZoneID;
		bitStream->Read(wZoneID);
		pGangZonePool->StopFlash(wZoneID);
	}
}

//----------------------------------------------------

void ScrApplyAnimation(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	BYTE byteAnimLibLen;
	BYTE byteAnimNameLen;
	char szAnimLib[256];
	char szAnimName[256];
	float fS;
	bool opt1, opt2, opt3, opt4;
	int  opt5;
	CPlayerPool *pPlayerPool = NULL;
	CPlayerPed *pPlayerPed = NULL;

	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	bitStream->Read(playerId);
	bitStream->Read(byteAnimLibLen);
	bitStream->Read(szAnimLib, byteAnimLibLen);
	bitStream->Read(byteAnimNameLen);
	bitStream->Read(szAnimName, byteAnimNameLen);
	bitStream->Read(fS);
	bitStream->Read(opt1);
	bitStream->Read(opt2);
	bitStream->Read(opt3);
	bitStream->Read(opt4);
	bitStream->Read(opt5);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool) {
		// Get the CPlayerPed for this player
		if (playerId == pPlayerPool->GetLocalPlayerID()) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			if (pPlayerPool->GetSlotState(playerId)) {
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
			}
		}
		if (pPlayerPed) {
			try {
				pPlayerPed->ApplyAnimation(szAnimName, szAnimLib, fS,
					(int)opt1, (int)opt2, (int)opt3, (int)opt4, (int)opt5);
			}
			catch (...) {}
		}
	}
}

//----------------------------------------------------

void ScrClearAnimations(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	bitStream->Read(playerId);
	MATRIX4X4 mat;

	CPlayerPool *pPlayerPool = NULL;
	CPlayerPed *pPlayerPed = NULL;

	pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool) {
		// Get the CPlayerPed for this player
		if (playerId == pPlayerPool->GetLocalPlayerID()) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			if (pPlayerPool->GetSlotState(playerId)) {
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
			}
		}
		if (pPlayerPed) {
			try {

				pPlayerPed->GetMatrix(&mat);
				pPlayerPed->TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);

			}
			catch (...) {}
		}
	}
}

//----------------------------------------------------

void ScrSetSpecialAction(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	BYTE byteSpecialAction;
	bitStream->Read(byteSpecialAction);

	CPlayerPool *pPool = pNetGame->GetPlayerPool();
	if (pPool) pPool->GetLocalPlayer()->ApplySpecialAction(byteSpecialAction);
}

//----------------------------------------------------

void ScrEnableStuntBonus(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	bool bStuntBonusEnabled;
	bitStream->Read(bStuntBonusEnabled);
	pGame->EnableStuntBonus(bStuntBonusEnabled);
}

//----------------------------------------------------

void ScrSetFightingStyle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMPLAYER playerId;
	BYTE byteFightingStyle = 0;

	bitStream->Read(playerId);
	bitStream->Read(byteFightingStyle);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CPlayerPed *pPlayerPed = 0;

	if (pPlayerPool) {
		if (playerId == pPlayerPool->GetLocalPlayerID()) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			if (pPlayerPool->GetSlotState(playerId)) {
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
			}
		}
		if (pPlayerPed) {
			try {

				pPlayerPed->SetFightingStyle(byteFightingStyle);

			}
			catch (...) {}
		}
	}
}

void ScrSetPlayerVelocity(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	VECTOR vecMoveSpeed;

	bitStream->Read(vecMoveSpeed.X);
	bitStream->Read(vecMoveSpeed.Y);
	bitStream->Read(vecMoveSpeed.Z);

	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

	if (pPlayerPed)
	{
		if (pPlayerPed->IsOnGround())
		{
			DWORD dwStateFlags = pPlayerPed->GetStateFlags();
			dwStateFlags ^= 3; // Make the game think the ped is off the ground so SetMoveSpeed works
			pPlayerPed->SetStateFlags(dwStateFlags);
		}

		pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
	}
}

//-----------------------------------------------------------

void ScrSetVehicleVelocity(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	VECTOR vecMoveSpeed;

	bitStream->Read(vecMoveSpeed.X);
	bitStream->Read(vecMoveSpeed.Y);
	bitStream->Read(vecMoveSpeed.Z);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

	if (pPlayerPed)
	{
		CVehicle* pVehicle = pVehiclePool->GetAt(pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle()));

		if (pVehicle)
			pVehicle->SetMoveSpeedVector(vecMoveSpeed);
	}
}

//-----------------------------------------------------------

void ScrCreateActor(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	BYTE byteNameLen;
	char szActorName[MAX_PLAYER_NAME];
	VECTOR vecPos;
	float fRotation;
	int iSkin;

	bitStream->Read(ActorID);
	bitStream->Read(byteNameLen);
	bitStream->Read(szActorName, byteNameLen);
	bitStream->Read(iSkin);
	bitStream->Read((char *)&vecPos, sizeof(VECTOR));
	bitStream->Read(fRotation);

	szActorName[byteNameLen] = 0;
	pNetGame->GetActorPool()->New(ActorID, iSkin, &vecPos, fRotation, szActorName);
}

//-----------------------------------------------------------

void ScrDestroyActor(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;

	bitStream->Read(ActorID);

	pNetGame->GetActorPool()->Delete(ActorID);
}

//-----------------------------------------------------------

void ScrMoveActorTo(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	int iMoveType;
	VECTOR vecPos;

	bitStream->Read(ActorID);
	bitStream->Read(iMoveType);
	bitStream->Read((char *)&vecPos, sizeof(VECTOR));

	CRemoteActor *pRemoteActor = pNetGame->GetActorPool()->GetAt(ActorID);

	if (pRemoteActor) {
		CActorPed *pActorPed = pRemoteActor->GetAtPed();
		if (pActorPed) pActorPed->MoveTo(vecPos, iMoveType);
	}
}

//-----------------------------------------------------------

void ScrActorKillPlayer(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	int iWeapon;
	SACMPLAYER playerId;

	bitStream->Read(ActorID);
	bitStream->Read(playerId);
	bitStream->Read(iWeapon);

	CRemoteActor *pRemoteActor = pNetGame->GetActorPool()->GetAt(ActorID);
	if (pRemoteActor && playerId && playerId != INVALID_PLAYER_ID && iWeapon) {
		CActorPed *pActorPed = pRemoteActor->GetAtPed();
		if (pActorPed) pActorPed->KillPlayer(playerId, iWeapon);
	}

}

//----------------------------------------------------

void ScrActorEnterVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	SACMVEHICLE VehicleID;
	bool bPassenger;

	bitStream->Read(ActorID);
	bitStream->Read(VehicleID);
	bitStream->Read(bPassenger);

	CActorPool *pActorPool = pNetGame->GetActorPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if (pActorPool && pVehiclePool) {
		CRemoteActor *pRemoteActor = pActorPool->GetAt(ActorID);
		CVehicle *pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pRemoteActor && pVehicle) {
			pRemoteActor->EnterVehicle(VehicleID, bPassenger);
		}
	}
}

//----------------------------------------------------

void ScrActorDriveVehicleToPoint(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	SACMVEHICLE VehicleID;
	VECTOR vecPoint;
	float fMaxSpeed;
	int iDriveType;
	float fAltitudeMin;
	float fAltitudeMax;

	bitStream->Read(ActorID);
	bitStream->Read(VehicleID);
	bitStream->Read(vecPoint.X);
	bitStream->Read(vecPoint.Y);
	bitStream->Read(vecPoint.Z);
	bitStream->Read(fMaxSpeed);
	bitStream->Read(iDriveType);
	bitStream->Read(fAltitudeMin);
	bitStream->Read(fAltitudeMax);

	CActorPool *pActorPool = pNetGame->GetActorPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if (pActorPool && pVehiclePool) {
		CRemoteActor *pRemoteActor = pActorPool->GetAt(ActorID);
		CVehicle *pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pRemoteActor && pVehicle) {
			if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_HELI)
				pRemoteActor->GetAtPed()->FlyHelicopterToPoint(pVehicle->m_dwGTAId, &vecPoint, fMaxSpeed, fAltitudeMin, fAltitudeMax);
			else
				pRemoteActor->GetAtPed()->DriveVehicleToPoint(pVehicle->m_dwGTAId, &vecPoint, fMaxSpeed, iDriveType);
		}
	}
}

//----------------------------------------------------

void ScrActorExitVehicle(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	SACMVEHICLE VehicleID;

	bitStream->Read(ActorID);
	bitStream->Read(VehicleID);

	CActorPool *pActorPool = pNetGame->GetActorPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if (pActorPool && pVehiclePool) {
		CRemoteActor *pRemoteActor = pActorPool->GetAt(ActorID);
		CVehicle *pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pRemoteActor && pVehicle) {
			pRemoteActor->ExitVehicle();
		}
	}
}

//----------------------------------------------------

void ScrSetActorPos(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMACTOR ActorID;
	VECTOR vecPos;

	bitStream->Read(ActorID);
	bitStream->Read(vecPos.X);
	bitStream->Read(vecPos.Y);
	bitStream->Read(vecPos.Z);

	CRemoteActor *pRemoteActor = pNetGame->GetActorPool()->GetAt(ActorID);

	if (pRemoteActor && pRemoteActor->GetAtPed()) {
		pRemoteActor->GetAtPed()->MoveTo(vecPos, MOVETO_DIRECT);
	}
}

//----------------------------------------------------

void ScrSetVehicleTireStatus(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	SACMVEHICLE vehicleID;
	BYTE byteTireStatus;
	BYTE byteTireID;
	bitStream->Read(vehicleID);
	bitStream->Read(byteTireID);
	bitStream->Read(byteTireStatus);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(vehicleID);

	if (pVehiclePool && pVehicle)
	{
		pVehicle->SetWheelPopped(byteTireID, byteTireStatus);
	}
}

//----------------------------------------------------

void ScrSetPlayerDrunkVisuals(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	int iVisuals;
	bitStream->Read(iVisuals);

	CPlayerPed* pPlayer = pGame->FindPlayerPed();
	if (pPlayer) {
		ScriptCommand(&set_player_drunk_visuals, 0, iVisuals);
	}
}

void ScrSetPlayerDrunkHandling(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{
	int iVisuals;
	bitStream->Read(iVisuals);

	CPlayerPed* pPlayer = pGame->FindPlayerPed();
	if (pPlayer) {
		ScriptCommand(&handling_responsiveness, 0, iVisuals);
	}
}

void RegisterScriptRPCs()
{
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetSpawnInfo, ScrSetSpawnInfo);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerTeam, ScrSetPlayerTeam);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerSkin, ScrSetPlayerSkin);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerName, ScrSetPlayerName);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerPos, ScrSetPlayerPos);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerPosFindZ, ScrSetPlayerPosFindZ);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerHealth, ScrSetPlayerHealth);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrPutPlayerInVehicle, ScrPutPlayerInVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrRemovePlayerFromVehicle, ScrRemovePlayerFromVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerColor, ScrSetPlayerColor);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrDisplayGameText, ScrDisplayGameText);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetInterior, ScrSetInterior);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetCameraPos, ScrSetCameraPos);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetCameraLookAt, ScrSetCameraLookAt);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetVehiclePos, ScrSetVehiclePos);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetVehicleZAngle, ScrSetVehicleZAngle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrVehicleParams, ScrVehicleParams);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetCameraBehindPlayer, ScrSetCameraBehindPlayer);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrTogglePlayerControllable, ScrTogglePlayerControllable);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrPlaySound, ScrPlaySound);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetWorldBounds, ScrSetWorldBounds);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrHaveSomeMoney, ScrHaveSomeMoney);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerFacingAngle, ScrSetPlayerFacingAngle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrResetMoney, ScrResetMoney);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrResetPlayerWeapons, ScrResetPlayerWeapons);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrGivePlayerWeapon, ScrGivePlayerWeapon);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrRespawnVehicle, ScrRespawnVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrLinkVehicle, ScrLinkVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerArmour, ScrSetPlayerArmour);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrDeathMessage, ScrDeathMessage);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetMapIcon, ScrSetMapIcon);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrDisableMapIcon, ScrDisableMapIcon);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetWeaponAmmo, ScrSetWeaponAmmo);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetGravity, ScrSetGravity);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetVehicleHealth, ScrSetVehicleHealth);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrAttachTrailerToVehicle, ScrAttachTrailerToVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrDetachTrailerFromVehicle, ScrDetachTrailerFromVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrCreateObject, ScrCreateObject);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetObjectPos, ScrSetObjectPos);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetObjectRotation, ScrSetObjectRotation);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrDestroyObject, ScrDestroyObject);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrCreateExplosion, ScrCreateExplosion);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrShowNameTag, ScrShowNameTag);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrMoveObject, ScrMoveObject);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrStopObject, ScrStopObject);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrNumberPlate, ScrNumberPlate);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrTogglePlayerSpectating, ScrTogglePlayerSpectating);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerSpectating, ScrSetPlayerSpectating);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrPlayerSpectatePlayer, ScrPlayerSpectatePlayer);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrPlayerSpectateVehicle, ScrPlayerSpectateVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrRemoveComponent, ScrRemoveComponent);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrForceSpawnSelection, ScrForceSpawnSelection);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrAttachObjectToPlayer, ScrAttachObjectToPlayer);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrInitMenu, ScrInitMenu);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrShowMenu, ScrShowMenu);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrHideMenu, ScrHideMenu);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerWantedLevel, ScrSetPlayerWantedLevel);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrShowTextDraw, ScrShowTextDraw);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrHideTextDraw, ScrHideTextDraw);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrEditTextDraw, ScrEditTextDraw);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrAddGangZone, ScrAddGangZone);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrRemoveGangZone, ScrRemoveGangZone);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrFlashGangZone, ScrFlashGangZone);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrStopFlashGangZone, ScrStopFlashGangZone);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrApplyAnimation, ScrApplyAnimation);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrClearAnimations, ScrClearAnimations);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetSpecialAction, ScrSetSpecialAction);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrEnableStuntBonus, ScrEnableStuntBonus);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetFightingStyle, ScrSetFightingStyle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerVelocity, ScrSetPlayerVelocity);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetVehicleVelocity, ScrSetVehicleVelocity);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrCreateActor, ScrCreateActor);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrDestroyActor, ScrDestroyActor);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrMoveActorTo, ScrMoveActorTo);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrActorKillPlayer, ScrActorKillPlayer);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrActorEnterVehicle, ScrActorEnterVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrActorExitVehicle, ScrActorExitVehicle);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrActorDriveVehicleToPoint, ScrActorDriveVehicleToPoint);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetActorPos, ScrSetActorPos);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetVehicleTireStatus, ScrSetVehicleTireStatus);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerDrunkVisuals, ScrSetPlayerDrunkVisuals);
	pNetGame->GetRPC()->RegisterFunction(RPC_ScrSetPlayerDrunkHandling, ScrSetPlayerDrunkHandling);
}

//----------------------------------------------------

void UnRegisterScriptRPCs()
{
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetSpawnInfo);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerTeam);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerSkin);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerName);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerPos);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerPosFindZ);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerHealth);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrPutPlayerInVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrRemovePlayerFromVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerColor);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrDisplayGameText);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetInterior);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetCameraPos);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetCameraLookAt);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetVehiclePos);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetVehicleZAngle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrVehicleParams);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetCameraBehindPlayer);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrTogglePlayerControllable);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrPlaySound);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetWorldBounds);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrHaveSomeMoney);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerFacingAngle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrResetMoney);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrResetPlayerWeapons);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrGivePlayerWeapon);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrRespawnVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrLinkVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerArmour);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrDeathMessage);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetMapIcon);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrDisableMapIcon);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetWeaponAmmo);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetGravity);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetVehicleHealth);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrAttachTrailerToVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrDetachTrailerFromVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrCreateObject);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetObjectPos);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetObjectRotation);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrDestroyObject);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrCreateExplosion);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrShowNameTag);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrMoveObject);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrStopObject);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrNumberPlate);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrTogglePlayerSpectating);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerSpectating);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrPlayerSpectatePlayer);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrPlayerSpectateVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrRemoveComponent);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrForceSpawnSelection);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrAttachObjectToPlayer);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrInitMenu);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrShowMenu);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrHideMenu);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerWantedLevel);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrShowTextDraw);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrHideTextDraw);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrEditTextDraw);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrAddGangZone);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrRemoveGangZone);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrFlashGangZone);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrStopFlashGangZone);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrApplyAnimation);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrClearAnimations);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetSpecialAction);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrEnableStuntBonus);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetFightingStyle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerVelocity);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetVehicleVelocity);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrCreateActor);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrDestroyActor);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrMoveActorTo);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrActorKillPlayer);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrActorEnterVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrActorExitVehicle);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrActorDriveVehicleToPoint);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetActorPos);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetVehicleTireStatus);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerDrunkVisuals);
	pNetGame->GetRPC()->UnregisterFunction(RPC_ScrSetPlayerDrunkHandling);
}

//----------------------------------------------------
