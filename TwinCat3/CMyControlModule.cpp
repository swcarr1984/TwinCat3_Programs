/*
Main Module Boilerplate Code
*/
// ================================================
// TwinCAT 3 C++ Control Program - Blocks only, stucture reference
// Beckhoff TcCOM Cyclic Module
// Sections: IO, State checks, Diagnostics, ADS mapping
// ================================================

#include "TcPch.h"           // TwinCAT precompiled header
#include "MyControlModule.h" // the module header

// ================================================
// SECTION 1: Constructor / Destructor
// ================================================
CMyControlModule::CMyControlModule()
{
    // Initialize variables, state machine, etc.
    m_nCounter = 0;
    m_eState = STATE_IDLE;
    m_bFault = FALSE;
    memset(&m_Inputs, 0, sizeof(m_Inputs));
    memset(&m_Outputs, 0, sizeof(m_Outputs));
    memset(&m_Diag, 0, sizeof(m_Diag));
}

CMyControlModule::~CMyControlModule()
{
    // Cleanup if needed
}

// ================================================
// SECTION 2: Cyclic Update (Main Control Loop)
// Called every task cycle - set within the tasks 
// when assigning module block
// ================================================
HRESULT CMyControlModule::CyclicUpdate()
{
    // Read inputs (automatically mapped)
    GetInputs();

    // ========================================
    // SECTION 2.1: Input / State Validation
    // ========================================
    BOOL bIoOk = TRUE;

    // Example state checks on IO
    if (m_Inputs.bStart && m_Inputs.bStop) {
        SetFault("Start and Stop both active - invalid");
        bIoOk = FALSE;
    }

    // Analog input safe/state oper check (base)
    if (m_Inputs.nAnalogIn > 60000) {  // Out of range
        SetFault("Analog input out of range");
        bIoOk = FALSE;
    }

    m_Diag.bIoHealthy = bIoOk;

    // ========================================
    // SECTION 2.2: Main State Machine
    // ========================================
    switch (m_eState) {
        case STATE_IDLE:
            m_Outputs.bRunning = FALSE;
            if (m_Inputs.bStart && bIoOk) {
                m_eState = STATE_RUNNING;
            }
            break;

        case STATE_RUNNING:
            m_Outputs.bRunning = TRUE;
            m_nCounter++;

            // control logic here
            m_Outputs.nAnalogOut = (UINT)(m_Inputs.nAnalogIn / 2);  // Simple map out

            if (m_Inputs.bStop || !bIoOk) {
                m_eState = STATE_FAULT;
            }
            break;

        case STATE_FAULT:
            m_Outputs.bRunning = FALSE;
            m_Outputs.bFault = TRUE;
            // Stay in fault until reset (!bStart)
            if (!m_Inputs.bStart) {
                ResetFault();
                m_eState = STATE_IDLE;
            }
            break;
    }

    // ========================================
    // SECTION 2.3: Diagnostics Update
    // ========================================
    m_Diag.ulCycleCount++;
    if (!bIoOk) m_Diag.ulErrorCount++;

    // ========================================
    // SECTION 2.4: Write to outputs
    // ========================================
    SetOutputs();

    return S_OK;
}

// ================================================
// SECTION 3: Helper Functions - IO, Faults, Diagnostics
// ================================================
void CMyControlModule::GetInputs()
{
    // TwinCAT automatically provides pointer to Inputs data area
    // ** Note: Access via member or ITcADI if needed
}

void CMyControlModule::SetOutputs()
{
    // Outputs written back automatically via data area
}

void CMyControlModule::SetFault(const char* pszReason)
{
    if (!m_bFault) {
        m_bFault = TRUE;
        // Log via TwinCAT trace or ADS
        TcTrace(TC_TRACE_LEVEL_ERROR, "Fault: %s", pszReason);
    }
}

void CMyControlModule::ResetFault()
{
    m_bFault = FALSE;
    m_Outputs.bFault = FALSE;
    m_Diag.ulErrorCount = 0;
}

// ================================================
// SECTION 4: Data Mapping to External Network (ADS Example)
// Use ADS for external communication (e.g., to HMI, SCADA, or another PC)
// ================================================
#include <AdsLib.h>  // or use TwinCAT ADS functions

// Example external: Publish diagnostics via ADS notification or server
HRESULT CMyControlModule::InitAds()
{
    // Example: Create an ADS port or use notification on data area
    // For simple cases- just map the Outputs/Diagnostics data area
    // in System Manager to external clients via ADS symbols.
    return S_OK;
}

// In CyclicUpdate or a method, you can write to an ADS variable on another device
// Or expose symbols for external read/write.

// Alternative: Use TcADS for internal or route to external AMS NetID