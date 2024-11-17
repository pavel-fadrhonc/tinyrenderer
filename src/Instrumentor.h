#pragma once

#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>
#include <thread>

namespace bEngine
{
    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    public:
        void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = name;
        }

        void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }

        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        Instrumentor() {}
        
        std::string m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount{0};
    };
    
    class InstrumentationTimer
    {
    public:
        using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>; 
        
        InstrumentationTimer(const char* name)
            : m_Name(name)
        {
            Start();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Start()
        {
            m_StartTimePoint = std::chrono::high_resolution_clock::now();
            m_Stopped = false;
        }

        void Stop()
        {
            Stop("");
        }
        
        void Stop(const char* message)
        {
            if (m_Stopped)
                return;

            auto endTimepoint = std::chrono::high_resolution_clock::now();
            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
            
            m_Stopped = true;

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

            std::string msg{m_Name};
            msg += message;
            
            Instrumentor::Get().WriteProfile({msg.c_str(), start, end, threadID});
        }

    private:
        const char* m_Name;
        bool m_Stopped {true};
        time_point m_StartTimePoint;
    };
}

#define PROFILE 1
#if PROFILE
    #define PROFILE_BEGIN_SESSION(name, filepath) ::bEngine::Instrumentor::Get().BeginSession(name, filepath);
    #define PROFILE_END_SESSION() ::bEngine::Instrumentor::Get().EndSession();
    #define PROFILE_SCOPE(name) ::bEngine::InstrumentationTimer timer##__LINE__(name);

    #define PROFILE_SCOPE_MESSAGE_START(name, timerIndex) ::bEngine::InstrumentationTimer timer##timerIndex(name);
    #define PROFILE_SCOPE_MESSAGE_STOP(message, timerIndex) timer##timerIndex.Stop(message);

    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
    #define PROFILE_BEGIN_SESSION(name, filepath) 
    #define PROFILE_FUNCTION() 
    #define PROFILE_SCOPE(name)
    #define PROFILE_END_SESSION() 
#endif