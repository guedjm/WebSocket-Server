#include <iostream>
#include <stdexcept>
#include "WebSocketFrame.h"

WebSocketFrame::WebSocketFrame()
{

}


WebSocketFrame::~WebSocketFrame()
{

}

void					WebSocketFrame::initFrame(unsigned char opCode)
{
	this->_FIN = 1;
	this->_RSV1 = 0;
	this->_RSV2 = 0;
	this->_RSV3 = 0;
	this->_mask = 0;
	this->_opcode = opCode;
}

void					WebSocketFrame::makeFrame(std::string& frame)
{
	unsigned int		tmp;
	unsigned char		nextPos;

	frame.resize(13);
	frame[0] = this->_FIN * 0x80;
	frame[0] += this->_RSV1 * 0x40;
	frame[0] += this->_RSV2 * 0x20;
	frame[0] += this->_RSV3 * 0x10;
	frame[0] += this->_opcode;
	frame[1] = this->_mask * 0x80;
	if (this->_payloadLen < 126)
	{
		frame[1] += this->_payloadLen;
		nextPos = 2;
	}
	else if (this->_payloadLen <= 65535)
	{
		frame[1] += 126;
		tmp = (this->_payloadLen & 0xFF00);
		frame[2] = tmp / 0xFF;
		frame[3] = this->_payloadLen - tmp;
		nextPos = 4;
	}
	else
	{
		throw std::runtime_error("WebSocketServer: Not implemented (frame size > 65535)");
		/*
		frame[1] += 127;
		frame[2] = (this->_payloadLen & 0xFF00000000000000) / 0xFFFFFFFFFFFFFF;
		frame[3] = (this->_payloadLen & 0xFF000000000000)   / 0xFFFFFFFFFFFF;
		frame[4] = (this->_payloadLen & 0xFF0000000000)     / 0xFFFFFFFFFF;
		frame[5] = (this->_payloadLen & 0xFF00000000)       / 0xFFFFFFFF;
		frame[6] = (this->_payloadLen & 0xFF000000)         / 0xFFFFFF;
		frame[7] = (this->_payloadLen & 0xFF0000)           / 0xFFFF;
		frame[8] = (this->_payloadLen & 0xFF00)             / 0xFF;
		frame[9] = this->_payloadLen & 0xFF;
		nextPos = 10;*/
	}
	if (this->_mask)
	{
		this->_maskingKey[0] = (MASKING_KEY & 0xFF000000) / 0xFFFFFF;
		this->_maskingKey[1] = (MASKING_KEY & 0xFF0000) / 0xFFFF;
		this->_maskingKey[2] = (MASKING_KEY & 0xFF00) / 0xFF;
		this->_maskingKey[3] = MASKING_KEY & 0xFF;
		frame[nextPos] = (MASKING_KEY & 0xFF000000) / 0xFFFFFF;
		frame[nextPos + 1] = (MASKING_KEY & 0xFF0000) / 0xFFFF;
		frame[nextPos + 2] = (MASKING_KEY & 0xFF00) / 0xFF;
		frame[nextPos + 3] = MASKING_KEY & 0xFF;
		nextPos += 4;
	}
	frame.resize(nextPos);
	if (this->_mask)
		this->maskPayload();
	//magie noire
	frame = frame.insert(nextPos, this->_payloadData);
}

void					WebSocketFrame::parseFrame(long int len, const std::string& frame)
{
	unsigned char			tmpLen;
	unsigned char			nextPos;
	const unsigned char		*cFrame = (const unsigned char*)frame.c_str();

	this->_FIN = (cFrame[0] & (1 << 0));
	this->_RSV1 = (cFrame[0] & (1 << 1));
	this->_RSV2 = (cFrame[0] & (1 << 2));
	this->_RSV3 = (cFrame[0] & (1 << 3));
	this->_opcode = (cFrame[0] & (0xF));
	this->_mask = (cFrame[1] & 0x80) / 0x80;
	tmpLen = (cFrame[1] & 0x7F);
	if (tmpLen < 126)
	{
		this->_payloadLen = tmpLen;
		nextPos = 2;
	}
	else if (tmpLen == 126)
	{
		this->_payloadLen = cFrame[2] * 256;
		this->_payloadLen += cFrame[3];
		nextPos = 4;
	}
	else
	{
		throw std::runtime_error("WebSocketServer :Not implemented (frame size > 65535)");
		/*this->_payloadLen = cFrame[2] * 72057594037927936;
		this->_payloadLen += cFrame[3] * 281474976710656;
		this->_payloadLen += cFrame[4] * 1099511627776;
		this->_payloadLen += cFrame[5] * 4294967296;
		this->_payloadLen += cFrame[6] * 16777216;
		this->_payloadLen += cFrame[7] * 65536;
		this->_payloadLen += cFrame[8] * 256;
		this->_payloadLen += cFrame[9];
		nextPos = 10;*/
	}
	if (this->_mask)
	{
		this->_maskingKey[0] = frame[nextPos];
		this->_maskingKey[1] = frame[nextPos + 1];
		this->_maskingKey[2] = frame[nextPos + 2];
		this->_maskingKey[3] = frame[nextPos + 3];
		nextPos += 4;
	}
	this->_payloadData = frame.substr(nextPos);
	if (this->_mask)
		this->unmaskPayload();
}

void					WebSocketFrame::maskPayload()
{
	for (unsigned long i = 0; i < this->_payloadLen; ++i)
	{
		this->_payloadData[i] ^= this->_maskingKey[i % 4];
	}
}

void					WebSocketFrame::unmaskPayload()
{
	for (unsigned long i = 0; i < this->_payloadLen; ++i)
	{
		this->_payloadData[i] ^= this->_maskingKey[i % 4];
	}
}

unsigned char			WebSocketFrame::getFIN() const
{
	return (_FIN);
}

unsigned char			WebSocketFrame::getRSV1() const
{
	return (_RSV1);
}

unsigned char			WebSocketFrame::getRSV2() const
{
	return (_RSV2);
}

unsigned char			WebSocketFrame::getRSV3() const
{
	return (_RSV3);
}

unsigned char			WebSocketFrame::getOpcode() const
{
	return (_opcode);
}

unsigned char			WebSocketFrame::getMask() const
{
	return (_mask);
}

unsigned int	WebSocketFrame::getLenPayload() const
{
	return (_payloadLen);
}

const std::string&		WebSocketFrame::getPayloadData() const
{
	return (_payloadData);
}

const std::string&		WebSocketFrame::getExtensionData() const
{
	return (_extensionData);
}

const std::string&		WebSocketFrame::getApplicationData() const
{
	return (_applicationData);
}

void					WebSocketFrame::setFIN(unsigned char FIN)
{
	this->_FIN = FIN;
}

void					WebSocketFrame::setRSV1(unsigned char RSV1)
{
	this->_RSV1 = RSV1;
}

void					WebSocketFrame::setRSV2(unsigned char RSV2)
{
	this->_RSV2 = RSV2;
}

void					WebSocketFrame::setRSV3(unsigned char RSV3)
{
	this->_RSV3 = RSV3;
}

void					WebSocketFrame::setOpcode(unsigned char opcode)
{
	this->_opcode = opcode;
}

void					WebSocketFrame::setMask(unsigned char mask)
{
	this->_mask = mask;
}

void					WebSocketFrame::setLenPayload(unsigned int payloadLen)
{
	this->_payloadLen = payloadLen;
}

void					WebSocketFrame::setPayloadData(const std::string& payloadData)
{
	this->_payloadData = payloadData;
}

void					WebSocketFrame::setExtensionData(const std::string& extensionData)
{
	this->_extensionData = extensionData;
}

void					WebSocketFrame::setApplicationData(const std::string& applicationData)
{
	this->_applicationData = applicationData;
}
