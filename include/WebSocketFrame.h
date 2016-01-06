#pragma once
#include <string>

#define MASKING_KEY 0x73776167 //#swag

class WebSocketFrame
{
public:
	WebSocketFrame();
	~WebSocketFrame();

	unsigned char getFIN() const;
	unsigned char getRSV1() const;
	unsigned char getRSV2() const;
	unsigned char getRSV3() const;
	unsigned char getOpcode() const;
	unsigned char getMask() const;
	unsigned int  getLenPayload() const;
	const std::string& getPayloadData() const;
	const std::string& getExtensionData() const;
	const std::string& getApplicationData() const;

	void setFIN(unsigned char);
	void setRSV1(unsigned char);
	void setRSV2(unsigned char);
	void setRSV3(unsigned char);
	void setOpcode(unsigned char);
	void setMask(unsigned char);
	void setLenPayload(unsigned int);
	void setPayloadData(const std::string&);
	void setExtensionData(const std::string&);
	void setApplicationData(const std::string&);

	void initFrame(unsigned char);
	void parseFrame(long int len, const std::string& frame);
	void makeFrame(std::string& frame);

private:
	void unmaskPayload();
	void maskPayload();

	unsigned char		_FIN;
	unsigned char		_RSV1;
	unsigned char		_RSV2;
	unsigned char		_RSV3;
	unsigned char		_opcode;
	unsigned char		_mask;
	unsigned int	_payloadLen;
	unsigned char		_maskingKey[4];
	std::string _payloadData;
	std::string _extensionData;
	std::string _applicationData;
};