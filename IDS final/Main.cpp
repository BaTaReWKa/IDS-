#include "IDS.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]  // ����������: ������ Attribute, �������� ������ STAThread
int main(cli::array<System::String^>^ args) // ���������� ������� ������ � �������� ^
{
    // ��������� ���������� ������
    Application::EnableVisualStyles();

    // ��������� ���������� ������
    Application::SetCompatibleTextRenderingDefault(false);

    // �������� � ������ �����
    IDSapp::IDS^ form = gcnew IDSapp::IDS();
    Application::Run(form);

    return 0;
}