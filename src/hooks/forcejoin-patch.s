###########################
#### Forcejoin patches ####
###########################

.global LBP1ForceJoinPatch
LBP1ForceJoinPatch:
# Start address : 0x03ab70c
    rlwinm r0, r25, 0x0, 0x18, 0x1f     # Move 'isLocalPlayerJoining' (local) to r0
    cmpwi cr7, r0, 0x0
    bne cr7, 0x3b 			            # If isLocalPlayerJoining == true goto success path (auto-accept) [0x03ab74c]
    
    lis r3, 0x86
    ori r3, r3, 0xd930				    # CNetworkManager pointer into r3
    lwz r3, 0x0(r3)					    # CNetworkManager struct into r3
    lwz r3, 0x18(r3)				    # CNetworkManager->FriendsManager into r3
    or r4, r27, r27				        # Load 'source' parameter into r4
    li r5, 0x0
    bla 0x128b8c					    # Call FindFriendEntryData()
    
    cmpwi cr7, r3, 0x0
    beq cr7, 0xd3			            # NULL friend entry; goto failure path [0x03ab808]
    lwz r6, 0x108(r3)				    # Load CFriendData->WeInvitedThisPlayerToJoinUs onto r6
    cmpwi cr7, r6, 0x0
    beq cr7, 0xc7			            # If WeInvitedThisPlayerToJoinUs == 0.0, goto failure path [0x03ab808]
    ori r0, r0, 0x0					    # !Required! NOP
    # Below is success path
# End address : 0x03ab748


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
    beq cr7, 0x10                       # Null-check [0x0285ff4]
    lwz r6, 0xe8(r3)                    # Load CFriendData->WeInvitedThisUserToJoinUs float into r6
    cmpwi cr7, r6, 0x0
    bgt cr7, 0x43                       # If WeInvitedThisUserToJoinUs > 0.0, goto success path (auto-accept) [0x0286030]
    
    # Below is _mostly_ base-game
    # with some addresses adjusted
    
    rldicl r3, r25, 0x0, 0x20           # r3 = CGameHostParties *this
    rldicl r4, r28, 0x0, 0x20           # r4 = 'message' parameter
    or r5, r27, r27                     # r5 = 'source' parameter
    li r6, 0x0                          # r6 = false (isLocalPlayerJoining)
    li r7, 0x0                          # r7 = NULL
    li r8, 0xc
    bla 0x285914                        # Call 'CGameHostParties::DealWithRequestJoinParty'
    
    rlwinm r3, r3, 0x0, 0x18, 0x1f
    cmpwi cr7, r3, 0x0
    bne cr7, -0xf0                      # DealWithRequestJoinParty success/fail check [0x0285f28]
    rldicl r3, r25, 0x0, 0x20
    rldicl r4, r28, 0x0, 0x20
    or r5, r27, r27
    bla 0x282440                        # Call 'CGameHostParties::ForwardRequestJoinPartyToLocalClient'
    b -0x104                            # [0x0285f28]
# End address : 0x028602c


.global LBP3ForceJoinPatch
LBP3ForceJoinPatch:
# Start address : 0x030561c
    cmpwi r28,0x0				        # isLocalPlayerJoining == false
    bne 0x33		                    # If it's true, skip to success path [0x0286030] 
    lis r3, 0xf8
    subic r3, r3, 0x5a80
    lwz r3, 0x1c(r3)		            # Load CNetworkManager->FriendsManager onto r3
    or r4, r31, r31		                # Load `source` param onto r4
    li r5, 0x0
    bla 0x2eb594			            # Call FindFriendDataEntry()
    cmpwi r3, 0x0
    beq 0x1b		                    # Go to failure path if null [0x0305658]
    lwz r3, 0xf0(r3)		            # Load CFriendData->WeInvitedThisPlayerToJoinUs
    cmpwi r3, 0x0
    beq 0xf		                        # Go to failure path if == 0.0 [0x0305658]

    # Success path [0x0286030]
    li r26, 0x1			                # Set 'skip_checks' to true
    b 0x8                               # [0x030565c]

    # Failure path [0x0305658]
    li r26, 0x0			                # Set 'skip_checks' to false

    # Execute func [0x030565c]
    ori r3, r29, 0x0
    ori r4, r30, 0x0
    ori r5, r31, 0x0
    ori r6, r28, 0x0
    or r7, r26, r26		                # r7 is the exact value of 'skip_checks' (0x1 = true, 0x0 = false)
    li r8, 0xd
    li r9, 0x6
    bla 0x305090			            # Call `DealWithRequestJoinParty`
    cmpwi r26, 0x0			            # Is 'skip_checks' set?
    bne 0x33
    cmpwi r3, 0x0                       # Begin Checks
    bne 0x2b
# End address : 0x0305688


.global LBP3JPForceJoinPatch
LBP3JPForceJoinPatch:
# Start address : 0x0301e34
    cmpwi r28,0x0				        # isLocalPlayerJoining == false
    bne 0x33		                    # If it's true, skip to success path [0x0301e68] 
    lis r3, 0xf6
    subic r3, r3, 0x6000
    lwz r3, 0x1c(r3)		            # Load CNetworkManager->FriendsManager onto r3
    or r4, r31, r31		                # Load `source` param onto r4
    li r5, 0x0
    bla 0x2e821c			            # Call FindFriendDataEntry()
    cmpwi r3, 0x0
    beq 0x1b		                    # Go to failure path if null [0x0301e70]
    lwz r3, 0xf0(r3)		            # Load CFriendData->WeInvitedThisPlayerToJoinUs
    cmpwi r3, 0x0
    beq 0xf		                        # Go to failure path if == 0.0 [0x0301e70]

    # Success path [0x0301e68]
    li r26, 0x1			                # Set 'skip_checks' to true
    b 0x8                               # [0x0301e74]

    # Failure path [0x0301e70]
    li r26, 0x0			                # Set 'skip_checks' to false

    # Execute func [0x0301e74]
    ori r3, r29, 0x0
    ori r4, r30, 0x0
    ori r5, r31, 0x0
    ori r6, r28, 0x0
    or r7, r26, r26		                # r7 is the exact value of 'skip_checks' (0x1 = true, 0x0 = false)
    li r8, 0xd
    li r9, 0x6
    bla 0x3018a8			            # Call `DealWithRequestJoinParty`
    cmpwi r26, 0x0			            # Is 'skip_checks' set?
    bne 0x33
    cmpwi r3, 0x0                       # Begin Checks
    bne 0x2b
# End address : 0x0301ea0