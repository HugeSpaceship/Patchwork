.global RNPCSRHook
RNPCSRHook:
    # r25 can probably be used to re-set r3, use r3 as our staging area
    lwz r3, 0(r3) # read pointer at r3 into r3
    lbz r3, 39(r3) # read the resource type from the structure

    cmpwi cr7, r3, 0xB # 0xB is the script type
    beq cr7, isscript # Branch to isscript if type is 0xB
    b notscript # Otherwise branch to notscript

isscript:
    ba 0x81B40 # Go to the error handling for when RNP isn't working

notscript:
    # set up registers for normal addToCSRQueue call
    clrldi r3, r25, 32 # Reset r3
    bla 0x7D1EC # AddToCSRQueue
    ba 0x81BCC # instruction after our hook branch

.global LBP2ScriptHook
LBP2ScriptHook:
    beq cr7, dontload # relocated branch from original function, uses a label here but is mostly equivalent

    # we shouldn't need to care about r0, as anything that uses it sets it immediately
    lwz r0, 0(r31) # r31 has the pointer to the CSR
    lbz r0, 36(r0) # 36 bytes in is the resource type
    cmpwi cr7, r0, 0x1 # if the resource is a script
    beq cr7, dontload # don't load it
    ba 0x143D78 # continue to original function

dontload:
    ba 0x153D3C # Return 0 in resource check function