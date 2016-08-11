function stats(fixentity, sessionID, message) {
  var sess = fixentity.getSession(sessionID);
  var ret = {
    sessionID: sessionID,
    senderSeqNum: sess.senderSeqNum,
    targetSeqNum: sess.targetSeqNum
  };
  if (message) {
    ret.message = message;
  }
  return ret;
}

function printStats(fixentity){
  var sessions = fixentity.getSessions();
  var sessionId = sessions[0];
  var sess = fixentity.getSession(sessionId);
  console.log('senderSeqNum', sess.senderSeqNum, 'targetSeqNum', sess.targetSeqNum);
}

module.exports = {
  stats: stats,
  printStats: printStats
};
