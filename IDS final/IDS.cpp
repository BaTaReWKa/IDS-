#include "IDS.h"
#include <fstream>

using namespace IDSapp;
using namespace System::IO::Ports;
using namespace System::Windows::Forms;

IDS::IDS(void)
{
	InitializeComponent();
	if (LicenseManager::UsageMode != LicenseUsageMode::Designtime)
	{
		SafeInitializeRuntime();
	}

	InitializeContextMenu();
}

IDS::~IDS()
{
	if (components)
		delete components;

	if (serialPort != nullptr && serialPort->IsOpen)
		serialPort->Close();
}

// Открытие порта для датчика
void IDS::SafeInitializeRuntime()
{
	std::ifstream in("Addresses.txt");
	if (!in.is_open()) return;

	std::string line, currentAddress;
	House* currentHouse = nullptr;

	while (std::getline(in, line))
	{
		if (line.find("[ADDRESS]") == 0)
		{
			currentAddress = line.substr(10); // после "[ADDRESS] "
			currentHouse = new House(currentAddress);
			HouseWrapper^ wrapper = gcnew HouseWrapper(currentHouse);
			ListOfAddresses->Items->Add(wrapper);
		}
		else if (line.find("[ID]") == 0 && currentHouse)
		{
			size_t idStart = 5;
			size_t pipePos = line.find("| [PLACE] ");
			if (pipePos == std::string::npos) continue;

			std::string id = line.substr(idStart, pipePos - idStart - 1); // -1 чтобы убрать пробел
			std::string place = line.substr(pipePos + 10); // после "| [PLACE] "

			currentHouse->addSensor(id, place);
		}
	}
	in.close();
}

void IDS::InitializeContextMenu()
{
	// Контекстное меню для домов
	addressContextMenu = gcnew System::Windows::Forms::ContextMenuStrip();
	deleteMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem("Удалить");
	deleteMenuItem->Click += gcnew System::EventHandler(this, &IDS::DeleteSelectedAddress);
	addressContextMenu->Items->Add(deleteMenuItem);
	ListOfAddresses->ContextMenuStrip = addressContextMenu;

	// Контекстное меню для датчиков
	sensorContextMenu = gcnew System::Windows::Forms::ContextMenuStrip();
	deleteSensorMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem("Удалить");
	deleteSensorMenuItem->Click += gcnew System::EventHandler(this, &IDS::DeleteSelectedSensor);
	sensorContextMenu->Items->Add(deleteSensorMenuItem);
	ListOfSensors->ContextMenuStrip = sensorContextMenu;
}

void IDS::ConnectSensor_Click(System::Object^ sender, System::EventArgs^ e) {
	try
	{
		if (serialPort == nullptr)
			serialPort = gcnew SerialPort();

		if (!serialPort->IsOpen)
		{
			serialPort->PortName = "COM11";
			serialPort->BaudRate = 9600;
			array<String^>^ ports = SerialPort::GetPortNames();
			if (Array::IndexOf(ports, "COM11") < 0)
			{
				MessageBox::Show("Порт COM11 не найден. Убедитесь, что устройство подключено.", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return;
			}

			serialPort->Open();

			if (distanceTimer == nullptr)
			{
				distanceTimer = gcnew Timer();
				distanceTimer->Interval = 2000;
				distanceTimer->Tick += gcnew EventHandler(this, &IDS::UpdateDistance);
				distanceTimer->Start();
			}
		}
	}
	catch (UnauthorizedAccessException^ ex)
	{
		MessageBox::Show("Доступ к порту запрещён: " + ex->Message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::IO::IOException^ ex)
	{
		MessageBox::Show("Ошибка ввода-вывода при доступе к порту: " + ex->Message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::ArgumentException^ ex)
	{
		MessageBox::Show("Некорректное имя порта: " + ex->Message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (Exception^ ex)
	{
		MessageBox::Show("Неизвестная ошибка: " + ex->Message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
}

// Добавление адреса в список
void IDS::LoadAddress_Click(System::Object^ sender, System::EventArgs^ e)
{
	String^ inputAddress = AddressInput->Text;

	if (!ValidateAddress(inputAddress)) {
		MessageBox::Show("Некорректный формат адреса!\nПример: ул. Ленина, 15а",
			"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	std::string stdAddr = marshal_as<std::string>(inputAddress);
	House* newHouse = new House(stdAddr);
	HouseWrapper^ wrapper = gcnew HouseWrapper(newHouse);

	ListOfAddresses->Items->Add(wrapper);
	AddressInput->Text = "";
	AddressInput->Focus();
}

// Обновление расстояния с датчика
void IDS::UpdateDistance(System::Object^ sender, System::EventArgs^ e)
{
	if (serialPort == nullptr || !serialPort->IsOpen || ListOfAddresses->Items->Count == 0)
		return;

	try {
		String^ line = serialPort->ReadLine()->Trim();
		Console::WriteLine("Получено из порта: " + line);
		int colonIndex = line->IndexOf(":");
		if (colonIndex < 0) {
			CurrentSensorID->Text = "Нет";
			LoadSensor->Enabled = false;
			return;
		}

		String^ sensorId = line->Substring(0, colonIndex);
		String^ distanceStr = line->Substring(colonIndex + 1);
		double distance = Convert::ToDouble(distanceStr);

		CurrentSensorID->Text = sensorId;
		LoadSensor->Enabled = true;

		Object^ selected = ListOfAddresses->SelectedItem;
		// обновляем расстояние для всех домов
		for (int i = 0; i < ListOfAddresses->Items->Count; ++i) {
			HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->Items[i]);
			if (hw != nullptr) {
				hw->GetNativeHouse()->setDistance(distance);
			}
		}
		if (selected != nullptr && ListOfAddresses->Items->Contains(selected))
			ListOfAddresses->SelectedItem = selected;

		for (int i = 0; i < ListOfAddresses->Items->Count; ++i) {
			HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->Items[i]);
			if (hw != nullptr) {
				House* house = hw->GetNativeHouse();
				if (house->hasSensor(marshal_as<std::string>(sensorId))) {
					house->updateDistance(marshal_as<std::string>(sensorId), distance);
					ListOfAddresses->Items[i] = hw; // обновление UI
				}
			}
		}

	}
	catch (...) {
		CurrentSensorID->Text = "Нет подключённого датчика";
		LoadSensor->Enabled = false;
	}
}

void IDS::ListOfSensors_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	if (ListOfAddresses->SelectedItem == nullptr || ListOfSensors->SelectedItem == nullptr)
		return;

	HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->SelectedItem);
	if (hw == nullptr) return;

	House* house = hw->GetNativeHouse();

	// Получаем ID из строки: ID: SNR_A1 | Место: Восток
	String^ selectedText = ListOfSensors->SelectedItem->ToString();
	int idStart = selectedText->IndexOf("ID: ") + 4;
	int idEnd = selectedText->IndexOf(" |");
	String^ id = selectedText->Substring(idStart, idEnd - idStart);

	std::string idStr = marshal_as<std::string>(id);
	if (!house->hasSensor(idStr)) return;

	std::shared_ptr<Sensor> sensor = house->getSensor(idStr);
	if (!sensor) return;

	if (sensor->isTimedOut()) {
		StatusOfSensor->Text = "Отключён";
	}
	else if (sensor->getLastDistance() < 50.0) {
		StatusOfSensor->Text = "Обнаружена сосулька!";
	}
	else {
		StatusOfSensor->Text = "Всё хорошо";
	}

}

void IDS::timer1_Tick(System::Object^ sender, System::EventArgs^ e)
{
	ListOfSensors_SelectedIndexChanged(nullptr, nullptr); // пересчитать статус
}


// Валидация адреса (проверка на правильность ввода)
bool IDS::ValidateAddress(String^ address)
{
	String^ pattern = L"^ул\\.\\s[А-Яа-яЁё]+(?:\\s[А-Яа-яЁё]+)*,\\s\\d+[А-Яа-яЁё]?$";
	Regex^ regex = gcnew Regex(pattern, RegexOptions::None);

	if (!regex->IsMatch(address))
		return false;

	for (int i = 0; i < ListOfAddresses->Items->Count; ++i)
	{
		HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->Items[i]);
		if (hw != nullptr)
		{
			String^ existingAddr = gcnew String(hw->GetNativeHouse()->getAddress().c_str());
			if (String::Compare(existingAddr, address, true) == 0)
				return false;
		}
	}

	return true;
}

// Добавление датчика к соответствующему адресу
void IDS::LoadSensor_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (ListOfAddresses->SelectedItem == nullptr)
	{
		MessageBox::Show("Выберите дом, к которому будет привязан датчик.", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	String^ sensorId = CurrentSensorID->Text;
	String^ sensorPlace = PlaceInput->Text->Trim();

	if (String::IsNullOrWhiteSpace(sensorId) || String::IsNullOrWhiteSpace(sensorPlace))
	{
		MessageBox::Show("Не удалось определить ID датчика. Подключите устройство.", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->SelectedItem);
	House* house = hw->GetNativeHouse();

	if (house->hasSensor(marshal_as<std::string>(sensorId)))
	{
		MessageBox::Show("Датчик с таким ID уже зарегистрирован для этого дома.", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	house->addSensor(
		marshal_as<std::string>(sensorId),
		marshal_as<std::string>(sensorPlace)
	);

	String^ entry = "ID: " + sensorId + " | Место: " + sensorPlace;
	Object^ selected = ListOfSensors->SelectedItem;
	ListOfSensors->Items->Add(entry);
	if (selected != nullptr && ListOfSensors->Items->Contains(selected))
		ListOfSensors->SelectedItem = selected;
	PlaceInput->Clear();
}

// Показ списка датчиков по выделенному адресу
void IDS::ListOfAddresses_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
	ListOfSensors->Items->Clear();

	HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->SelectedItem);
	if (hw == nullptr) return;

	auto sensors = hw->GetNativeHouse()->getSensors();
	for (const auto& pair : sensors)
	{
		String^ id = gcnew String(pair.first.c_str());
		String^ place = gcnew String(pair.second->getPlace().c_str());

		ListOfSensors->Items->Add("ID: " + id + " | Место: " + place);
	}

}

// Поиск адреса в списке
void IDS::SearchInput_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
	String^ search = SearchInput->Text->Trim();

	for (int i = 0; i < ListOfAddresses->Items->Count; ++i)
	{
		HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->Items[i]);
		if (hw != nullptr)
		{
			String^ addr = gcnew String(hw->GetNativeHouse()->getAddress().c_str());

			if (addr->IndexOf(search, StringComparison::OrdinalIgnoreCase) >= 0)
			{
				ListOfAddresses->SelectedIndex = i;
				return;
			}
		}
	}

	ListOfAddresses->ClearSelected();
}

void IDS::IDS_FormClosing(System::Object^ sender, FormClosingEventArgs^ e)
{
	std::ofstream out("Addresses.txt");
	if (!out.is_open()) return;

	for (int i = 0; i < ListOfAddresses->Items->Count; ++i)
	{
		HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->Items[i]);
		if (hw == nullptr) continue;

		House* house = hw->GetNativeHouse();
		out << "[ADDRESS] " << house->getAddress() << "\n";

		for (const auto& pair : house->getSensors())
		{
			out << "[ID] " << pair.first << " | [PLACE] " << pair.second->getPlace() << "\n";

		}
		out << "\n";
	}
	out.close();
}

void IDS::ListOfSensors_RightClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{
	if (e->Button == ::MouseButtons::Right)
	{
		int index = ListOfSensors->IndexFromPoint(e->Location);
		if (index != ListBox::NoMatches)
		{
			ListOfSensors->SelectedIndex = index;
		}
	}
}


void IDS::DeleteSelectedSensor(System::Object^ sender, System::EventArgs^ e)
{
	int selectedIndex = ListOfSensors->SelectedIndex;
	if (selectedIndex < 0 || ListOfAddresses->SelectedItem == nullptr)
		return;

	// Получаем ID датчика из строки, например: "ID: 123 | Место: крыша"
	String^ selectedEntry = ListOfSensors->Items[selectedIndex]->ToString();
	Regex^ idRegex = gcnew Regex("ID:\\s*(\\S+)");
	Match^ match = idRegex->Match(selectedEntry);
	if (!match->Success)
		return;

	String^ sensorId = match->Groups[1]->Value;

	// Удаляем из модели
	HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->SelectedItem);
	if (hw != nullptr)
	{
		hw->GetNativeHouse()->removeSensor(marshal_as<std::string>(sensorId));
	}

	// Удаляем из интерфейса
	ListOfSensors->Items->RemoveAt(selectedIndex);
}

void IDS::ListOfAddresses_RightClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{
	if (e->Button == ::MouseButtons::Right)
	{
		int index = ListOfAddresses->IndexFromPoint(e->Location);
		if (index != ListBox::NoMatches)
		{
			ListOfAddresses->SelectedIndex = index; // выделить элемент
		}
	}
}

void IDS::DeleteSelectedAddress(System::Object^ sender, System::EventArgs^ e)
{
	int selectedIndex = ListOfAddresses->SelectedIndex;
	if (selectedIndex >= 0)
	{
		HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->Items[selectedIndex]);
		if (hw != nullptr)
		{
			delete hw->GetNativeHouse(); // очистка памяти
		}

		ListOfAddresses->Items->RemoveAt(selectedIndex);
		ListOfSensors->Items->Clear();
	}
}

