#import "import/stlvector.h"

struct PropInfoPojo {
	std::string propId;
	std::string propName;
};
struct TermInfoPojo {
	std::string termId;
	std::string termName;
	std::string termIp;
	std::string propId;
};

struct SyndwsadataResponse {
	std::string responseVersion;
	std::string requestCtxUUID;
	std::string responseCode;
	std::string responseExtCode;
	std::string responseMessage;
	std::vector<struct PropInfoPojo> lstPropInfo;
	std::vector<struct TermInfoPojo> lstTermInfo;
};

struct SyndwsadataRequest {
	std::string requestVersion;
	std::string requestCtxUUID;
	std::string requestUserCode;
	std::string requestUserPwd;
	std::string requestUserName;
	std::string requestAppCls;
	std::string requestAppInst;
};
int ns__Syndwsadata(struct SyndwsadataRequest request, struct SyndwsadataResponse* serviceResponse);
//int ns__syndwsadata(struct SyndwsadataRequest request, struct SyndwsadataResponse* serviceResponse);


struct SyndwsaTicketInfoRequest {
	std::string wtCode;
	std::string wtPlanStartTime;
	std::string wtPlanEndTime;
	//std::string reason;
	std::vector<struct PropInfoPojo> lstPropInfo;
	std::vector<struct TermInfoPojo> lstTermInfo;
};
struct SyndwsaTicketInfoResponse {
	std::string responseVersion;
	std::string requestCtxUUID;
	std::string responseCode;
	std::string responseExtCode;
	std::string responseMessage;
};

int ns__syndwsaTicketInfo(struct SyndwsaTicketInfoRequest request, struct SyndwsaTicketInfoResponse* serviceResponse);


