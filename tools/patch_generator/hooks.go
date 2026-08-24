package patch_generator

import (
	"encoding/binary"
	"fmt"
	"io"
)

const baMask uint32 = 0x48000002

func GetHook(r io.ReaderAt, hookOffset uint64) ([]byte, error) {
	baFound := false
	i := hookOffset
	outBuf := make([]byte, 0, 64)
	insBuf := make([]byte, 4)
	for {
		n, err := r.ReadAt(insBuf, int64(i))
		if err != nil {
			return nil, err
		}
		if n != 4 {
			return nil, fmt.Errorf("unexpected read size %d", n)
		}
		outBuf = append(outBuf, insBuf...)
		instruction := binary.BigEndian.Uint32(insBuf)
		if instruction&baMask == baMask && insBuf[3]&1 != 1 { // if the last bit is 1 it's a linked branch
			if baFound { // on the second BA, write out the data
				return outBuf, nil
			}
			baFound = true
		}
		i += 4
	}
}
