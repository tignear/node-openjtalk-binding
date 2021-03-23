const {synthesis} =require("./addon");
synthesis("にゃーん",{
  htsvoice: process.env.OPEN_JTALK_HTSVOICE,
  dictionary: process.env.OPEN_JTALK_DIC
});