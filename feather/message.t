fgMsgType = uint16

struct fgMessage {
  type : fgMsgType;
  subtype : fgMsgType;
  union {
    construct : &opaque;
  }
}

struct fgMessageResult {
  union {
    error : fgError;
    ptr : &opaque;
    construct : uint;
  }
}

-- fgBehaviorFunction = {fgRoot&, fgElement&, fgMessage&} -> fgMessageResult

