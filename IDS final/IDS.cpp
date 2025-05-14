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

// �������� ����� ��� �������
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
			currentAddress = line.substr(10); // ����� "[ADDRESS] "
			currentHouse = new House(currentAddress);
			HouseWrapper^ wrapper = gcnew HouseWrapper(currentHouse);
			ListOfAddresses->Items->Add(wrapper);
		}
		else if (line.find("[ID]") == 0 && currentHouse)
		{
			size_t idStart = 5;
			size_t pipePos = line.find("| [PLACE] ");
			if (pipePos == std::string::npos) continue;

			std::string id = line.substr(idStart, pipePos - idStart - 1); // -1 ����� ������ ������
			std::string place = line.substr(pipePos + 10); // ����� "| [PLACE] "

			currentHouse->addSensor(id, place);
		}
	}
	in.close();
}

void IDS::InitializeContextMenu()
{
	// ����������� ���� ��� �����
	addressContextMenu = gcnew System::Windows::Forms::ContextMenuStrip();
	deleteMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem("�������");
	deleteMenuItem->Click += gcnew System::EventHandler(this, &IDS::DeleteSelectedAddress);
	addressContextMenu->Items->Add(deleteMenuItem);
	ListOfAddresses->ContextMenuStrip = addressContextMenu;

	// ����������� ���� ��� ��������
	sensorContextMenu = gcnew System::Windows::Forms::ContextMenuStrip();
	deleteSensorMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem("�������");
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
				MessageBox::Show("���� COM11 �� ������. ���������, ��� ���������� ����������.", "������", MessageBoxButtons::OK, MessageBoxIcon::Warning);
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
		MessageBox::Show("������ � ����� ��������: " + ex->Message, "������", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::IO::IOException^ ex)
	{
		MessageBox::Show("������ �����-������ ��� ������� � �����: " + ex->Message, "������", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::ArgumentException^ ex)
	{
		MessageBox::Show("������������ ��� �����: " + ex->Message, "������", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (Exception^ ex)
	{
		MessageBox::Show("����������� ������: " + ex->Message, "������", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
}

// ���������� ������ � ������
void IDS::LoadAddress_Click(System::Object^ sender, System::EventArgs^ e)
{
	String^ inputAddress = AddressInput->Text;

	if (!ValidateAddress(inputAddress)) {
		MessageBox::Show("������������ ������ ������!\n������: ��. ������, 15�",
			"������", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	std::string stdAddr = marshal_as<std::string>(inputAddress);
	House* newHouse = new House(stdAddr);
	HouseWrapper^ wrapper = gcnew HouseWrapper(newHouse);

	ListOfAddresses->Items->Add(wrapper);
	AddressInput->Text = "";
	AddressInput->Focus();
}

// ���������� ���������� � �������
void IDS::UpdateDistance(System::Object^ sender, System::EventArgs^ e)
{
	if (serialPort == nullptr || !serialPort->IsOpen || ListOfAddresses->Items->Count == 0)
		return;

	try {
		String^ line = serialPort->ReadLine()->Trim();
		Console::WriteLine("�������� �� �����: " + line);
		int colonIndex = line->IndexOf(":");
		if (colonIndex < 0) {
			CurrentSensorID->Text = "���";
			LoadSensor->Enabled = false;
			return;
		}

		String^ sensorId = line->Substring(0, colonIndex);
		String^ distanceStr = line->Substring(colonIndex + 1);
		double distance = Convert::ToDouble(distanceStr);

		CurrentSensorID->Text = sensorId;
		LoadSensor->Enabled = true;

		Object^ selected = ListOfAddresses->SelectedItem;
		// ��������� ���������� ��� ���� �����
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
					ListOfAddresses->Items[i] = hw; // ���������� UI
				}
			}
		}

	}
	catch (...) {
		CurrentSensorID->Text = "��� ������������� �������";
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

	// �������� ID �� ������: ID: SNR_A1 | �����: ������
	String^ selectedText = ListOfSensors->SelectedItem->ToString();
	int idStart = selectedText->IndexOf("ID: ") + 4;
	int idEnd = selectedText->IndexOf(" |");
	String^ id = selectedText->Substring(idStart, idEnd - idStart);

	std::string idStr = marshal_as<std::string>(id);
	if (!house->hasSensor(idStr)) return;

	std::shared_ptr<Sensor> sensor = house->getSensor(idStr);
	if (!sensor) return;

	if (sensor->isTimedOut()) {
		StatusOfSensor->Text = "��������";
	}
	else if (sensor->getLastDistance() < 50.0) {
		StatusOfSensor->Text = "���������� ��������!";
	}
	else {
		StatusOfSensor->Text = "�� ������";
	}

}

void IDS::timer1_Tick(System::Object^ sender, System::EventArgs^ e)
{
	ListOfSensors_SelectedIndexChanged(nullptr, nullptr); // ����������� ������
}


// ��������� ������ (�������� �� ������������ �����)
bool IDS::ValidateAddress(String^ address)
{
	String^ pattern = L"^��\\.\\s[�-��-���]+(?:\\s[�-��-���]+)*,\\s\\d+[�-��-���]?$";
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

// ���������� ������� � ���������������� ������
void IDS::LoadSensor_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (ListOfAddresses->SelectedItem == nullptr)
	{
		MessageBox::Show("�������� ���, � �������� ����� �������� ������.", "������", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	String^ sensorId = CurrentSensorID->Text;
	String^ sensorPlace = PlaceInput->Text->Trim();

	if (String::IsNullOrWhiteSpace(sensorId) || String::IsNullOrWhiteSpace(sensorPlace))
	{
		MessageBox::Show("�� ������� ���������� ID �������. ���������� ����������.", "������", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->SelectedItem);
	House* house = hw->GetNativeHouse();

	if (house->hasSensor(marshal_as<std::string>(sensorId)))
	{
		MessageBox::Show("������ � ����� ID ��� ��������������� ��� ����� ����.", "������", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	house->addSensor(
		marshal_as<std::string>(sensorId),
		marshal_as<std::string>(sensorPlace)
	);

	String^ entry = "ID: " + sensorId + " | �����: " + sensorPlace;
	Object^ selected = ListOfSensors->SelectedItem;
	ListOfSensors->Items->Add(entry);
	if (selected != nullptr && ListOfSensors->Items->Contains(selected))
		ListOfSensors->SelectedItem = selected;
	PlaceInput->Clear();
}

// ����� ������ �������� �� ����������� ������
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

		ListOfSensors->Items->Add("ID: " + id + " | �����: " + place);
	}

}

// ����� ������ � ������
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

	// �������� ID ������� �� ������, ��������: "ID: 123 | �����: �����"
	String^ selectedEntry = ListOfSensors->Items[selectedIndex]->ToString();
	Regex^ idRegex = gcnew Regex("ID:\\s*(\\S+)");
	Match^ match = idRegex->Match(selectedEntry);
	if (!match->Success)
		return;

	String^ sensorId = match->Groups[1]->Value;

	// ������� �� ������
	HouseWrapper^ hw = dynamic_cast<HouseWrapper^>(ListOfAddresses->SelectedItem);
	if (hw != nullptr)
	{
		hw->GetNativeHouse()->removeSensor(marshal_as<std::string>(sensorId));
	}

	// ������� �� ����������
	ListOfSensors->Items->RemoveAt(selectedIndex);
}

void IDS::ListOfAddresses_RightClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{
	if (e->Button == ::MouseButtons::Right)
	{
		int index = ListOfAddresses->IndexFromPoint(e->Location);
		if (index != ListBox::NoMatches)
		{
			ListOfAddresses->SelectedIndex = index; // �������� �������
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
			delete hw->GetNativeHouse(); // ������� ������
		}

		ListOfAddresses->Items->RemoveAt(selectedIndex);
		ListOfSensors->Items->Clear();
	}
}

