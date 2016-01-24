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
  var sess = fixentity.getSession(fixentity.getSessions()[0]);
  console.log('senderSeqNum', sess.senderSeqNum, 'targetSeqNum', sess.targetSeqNum);
}

module.exports = {
  stats: stats,
  printStats: printStats
};
