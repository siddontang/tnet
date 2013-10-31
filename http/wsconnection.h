#ifndef _WSCONNECTION_H_
#define _WSCONNECTION_H_

#include <tr1/memory>
#include <tr1/functional>

#include <string>
#include <vector>
#include <stdint.h>

#include "wsevent.h"

namespace tnet
{
    //now we wiil only support rfc6455
    //refer to tornado websocket implementation

    class Connection;
    class HttpRequest;

    class WsConnection
    {
    public:
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        typedef std::tr1::weak_ptr<Connection> WeakConnectionPtr_t;

        typedef std::tr1::function<void (const ConnectionPtr_t&, WsEvent, const std::string)> EventCallback_t;

        WsConnection(const EventCallback_t& func);
        ~WsConnection();
    
        int onHandshake(const ConnectionPtr_t& conn, const HttpRequest& request);

        ssize_t onRead(const ConnectionPtr_t& conn, const char* data, size_t count);

        static void ping(const ConnectionPtr_t& conn, const std::string& message);
        static void send(const ConnectionPtr_t& conn, const std::string& message, bool binary = false);
        static void close(const ConnectionPtr_t& conn);
   
        bool textFrame() { m_opcode == 0 ? (m_lastOpcode == 0x1) : (m_opcode == 0x1); }
        bool binaryFrame() { m_opcode == 0 ? (m_lastOpcode == 0x2) : (m_opcode == 0x2); }

    private:
        void handleError(const ConnectionPtr_t& conn, int statusCode, const std::string& message = "");
        int checkHeader(const ConnectionPtr_t& conn, const HttpRequest& request);

        bool isFinalFrame() { return m_final; }
        bool isMaskFrame() { return m_mask; }
   
        bool isControlFrame() { return m_opcode & 0x08; }
        
        ssize_t onFrameStart(const char* data, size_t count);
        ssize_t onFramePayloadLen(const char* data, size_t count);
        ssize_t onFramePayloadLen16(const char* data, size_t count);
        ssize_t onFramePayloadLen64(const char* data, size_t count);

        ssize_t onFrameMaskingKey(const char* data, size_t count);
        ssize_t onFrameData(const char* data, size_t count);

        ssize_t handleFramePayloadLen(size_t payloadLen);
        ssize_t handleFrameData(const ConnectionPtr_t& conn);
        ssize_t handleMessage(const ConnectionPtr_t& conn, uint8_t opcode, const std::string& message);
        ssize_t tryRead(const char* data, size_t count, size_t tryReadData);
    
        static void sendFrame(const ConnectionPtr_t& conn, bool finalFrame, char opcode, const std::string& message = std::string());

    private:
        enum FrameStatus
        {
            FrameStart,
            FramePayloadLen,
            FramePayloadLen16,
            FramePayloadLen64,
            FrameMaskingKey,
            FrameData,
            FrameFinal,
            FrameError,
        }; 

        std::string m_frame;
    
        size_t m_payloadLen;

        FrameStatus m_status;

        uint8_t m_maskingKey[4];

        uint8_t m_final;
        uint8_t m_opcode;
        uint8_t m_mask;
        uint8_t m_lastOpcode;
    
        std::string m_cache;
    
        EventCallback_t m_func;
    
        static bool ms_maskOutgoing;
    };    
}

#endif
