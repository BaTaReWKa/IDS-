#include "IDS.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]  // Исправлено: убрано Attribute, оставлен только STAThread
int main(cli::array<System::String^>^ args) // Исправлены угловые скобки и добавлен ^
{
    // Включение визуальных стилей
    Application::EnableVisualStyles();

    // Настройка рендеринга текста
    Application::SetCompatibleTextRenderingDefault(false);

    // Создание и запуск формы
    IDSapp::IDS^ form = gcnew IDSapp::IDS();
    Application::Run(form);

    return 0;
}