###########################
#### Forcejoin patches ####
###########################

.global LBP2ForceJoinPatch
LBP2ForceJoinPatch:
# Start address : 0x0285fb8
    rlwinm r0, r23, 0x0, 0x18, 0x1f     # Move 'isLocalPlayerJoining' (local) to r0
    cmpwi cr7, r0, 0x0
    bne cr7, 0x73                       # If isLocalPlayerJoining == true, goto success path (auto-accept) [0x0286030]
    
    lis r3, 0xd9
    ori r3, r3, 0xb35c                  # Load GNetworkManager pointer into r3
    lwz r3, 0x0(r3)                     # Load GNetworkManager struct into r3
    lwz r3, 0x1c(r3)                    # Load GNetworkManager->FriendManager into r3
    or r4, r27, r27                     # Load 'source' parameter from r27 into r4
    li r5, 0x0
    bla 0x264148                        # Call 'CNetworkFriendsManager::FindFriendDataEntry(FriendManager *this, NetworkClientId friend, null)'

    cmpwi cr7, r3, 0x0
    beq cr7, 0x0c                       # Null-check [0x0285ff4]
    lwz r6, 0xe8(r3)                    # Load CFriendData->WeInvitedThisUserToJoinUs float into r6
    cmpwi cr7, r6, 0x0
    bgt cr7, 0x43                       # If WeInvitedThisUserToJoinUs > 0.0, goto success path (auto-accept) [0x0286030]
    
    # Below is _mostly_ base-game
    # with some addresses adjusted
    
    rldicl r3, r25, 0x0, 0x20           # r3 = CGameHostParties *this
    rldicl r4, r28, 0x0, 0x20           # r4 = 'message' parameter
    or r5, r27, r27                     # r5 = 'source' parameter
    li r7, 0x0                          # r6/r7 = NULL (r6 is already 0x0 here as a result of the cmpwi check above, so we save 4 bytes)
    li r8, 0xc
    bla 0x285914                        # Call 'CGameHostParties::DealWithRequestJoinParty'
    
    rlwinm r3, r3, 0x0, 0x18, 0x1f
    cmpwi cr7, r3, 0x0
    bne cr7, -0xec                      # DealWithRequestJoinParty success/fail check [0x0285f28]
    rldicl r3, r25, 0x0, 0x20
    rldicl r4, r28, 0x0, 0x20
    or r5, r27, r27
    bla 0x282440                        # Call 'CGameHostParties::ForwardRequestJoinPartyToLocalClient'
    b -0x100                            # [0x0285f28]
    ori r0, r0, 0x0                     # !Required! NOP
# End address : 0x028602c