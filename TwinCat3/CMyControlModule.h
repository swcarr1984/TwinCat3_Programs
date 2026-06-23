/*
Header File Sketch (MyControlModule.h)
*/
#pragma once
#include "TcCom.h"  // Base classes

enum EControlState {
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_FAULT
};

class CMyControlModule : public CTcComObject, public ITcCyclic
{
public:
    CMyControlModule();
    virtual ~CMyControlModule();

    // ITcCyclic
    virtual HRESULT TCOMAPI CyclicUpdate();

private:
    // Data areas (linked via TMC)
    Inputs m_Inputs;
    Outputs m_Outputs;
    Diagnostics m_Diag;

    ULONG m_nCounter;
    EControlState m_eState;
    BOOL m_bFault;

    void GetInputs();
    void SetOutputs();
    void SetFault(const char* pszReason);
    void ResetFault();
    HRESULT InitAds();  // For external network mapping
};