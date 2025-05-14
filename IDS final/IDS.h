#pragma once
#include <Windows.h>
#include <msclr/marshal_cppstd.h>
#include "House.h"

namespace IDSapp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO::Ports;
	using namespace System::Text::RegularExpressions;
	using namespace msclr::interop;

	public ref class HouseWrapper
	{
	private:
		House* nativeHouse;

	public:
		HouseWrapper(House* h) {
			nativeHouse = h;
		}

		~HouseWrapper() { this->!HouseWrapper(); }
		!HouseWrapper() { delete nativeHouse; }


		House* GetNativeHouse() {
			return nativeHouse;
		}

		virtual String^ ToString() override {
			std::string addr = nativeHouse->getAddress();
			double dist = nativeHouse->getDistance();
			return gcnew String(addr.c_str());
		}
	};

	/// <summary>
	/// Сводка для IDS
	/// </summary>
	public ref class IDS : public System::Windows::Forms::Form
	{
	public:
		IDS(void);

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~IDS();

	private:
		SerialPort^ serialPort;
		Timer^ distanceTimer;
		void SafeInitializeRuntime();
		void InitializeContextMenu();
		void LoadAddress_Click(System::Object^ sender, System::EventArgs^ e);
		void UpdateDistance(System::Object^ sender, System::EventArgs^ e);
		bool ValidateAddress(String^ address);
		void LoadSensor_Click(System::Object^ sender, System::EventArgs^ e);
		void ListOfAddresses_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		Void SearchInput_TextChanged(System::Object^ sender, System::EventArgs^ e);
		Void IDS_FormClosing(System::Object^ sender, FormClosingEventArgs^ e);
		void ListOfSensors_RightClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		void DeleteSelectedSensor(System::Object^ sender, System::EventArgs^ e);
		void ListOfAddresses_RightClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		void DeleteSelectedAddress(System::Object^ sender, System::EventArgs^ e);
		void ListOfSensors_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		void timer1_Tick(System::Object^ sender, System::EventArgs^ e);
		void ConnectSensor_Click(System::Object^ sender, System::EventArgs^ e);

	private: System::Windows::Forms::ListBox^ ListOfAddresses;
	private: System::Windows::Forms::TextBox^ AddressInput;
	private: System::Windows::Forms::Button^ LoadAddress;
	private: System::Windows::Forms::Label^ Addresses;
	private: System::Windows::Forms::TextBox^ SearchInput;
	private: System::Windows::Forms::ListBox^ ListOfSensors;
	private: System::Windows::Forms::Label^ LabelID;
	private: System::Windows::Forms::Label^ LabelPlace;
	private: System::Windows::Forms::TextBox^ PlaceInput;
	private: System::Windows::Forms::Label^ LabelSensors;
	private: System::Windows::Forms::Button^ LoadSensor;
	private: System::Windows::Forms::ContextMenuStrip^ addressContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^ deleteMenuItem;
	private: System::Windows::Forms::ContextMenuStrip^ sensorContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^ deleteSensorMenuItem;
	private: System::Windows::Forms::Label^ CurrentSensorID;
	private: System::Windows::Forms::Label^ SensorIDLabel;
	private: System::Windows::Forms::Label^ LabelStatusOfSensor;
	private: System::Windows::Forms::Label^ StatusOfSensor;
	private: System::DateTime lastSensorUpdate;
	private: System::Windows::Forms::Button^ ConnectSensor;




		   /// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->ListOfAddresses = (gcnew System::Windows::Forms::ListBox());
			this->AddressInput = (gcnew System::Windows::Forms::TextBox());
			this->LoadAddress = (gcnew System::Windows::Forms::Button());
			this->Addresses = (gcnew System::Windows::Forms::Label());
			this->SearchInput = (gcnew System::Windows::Forms::TextBox());
			this->ListOfSensors = (gcnew System::Windows::Forms::ListBox());
			this->LabelID = (gcnew System::Windows::Forms::Label());
			this->LabelPlace = (gcnew System::Windows::Forms::Label());
			this->PlaceInput = (gcnew System::Windows::Forms::TextBox());
			this->LabelSensors = (gcnew System::Windows::Forms::Label());
			this->LoadSensor = (gcnew System::Windows::Forms::Button());
			this->SensorIDLabel = (gcnew System::Windows::Forms::Label());
			this->CurrentSensorID = (gcnew System::Windows::Forms::Label());
			this->addressContextMenu = gcnew System::Windows::Forms::ContextMenuStrip();
			this->LabelStatusOfSensor = (gcnew System::Windows::Forms::Label());
			this->StatusOfSensor = (gcnew System::Windows::Forms::Label());
			this->deleteMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem("Удалить");
			this->addressContextMenu->Items->Add(this->deleteMenuItem);
			this->ConnectSensor = (gcnew System::Windows::Forms::Button());
			this->deleteMenuItem->Click += gcnew System::EventHandler(this, &IDS::DeleteSelectedAddress);
			this->ListOfAddresses->ContextMenuStrip = this->addressContextMenu;
			this->ListOfAddresses->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &IDS::ListOfAddresses_RightClick);
			this->SuspendLayout();
			

			// 
			// ListOfAddresses
			// 
			this->ListOfAddresses->Font = (gcnew System::Drawing::Font(L"Times New Roman", 13.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ListOfAddresses->FormattingEnabled = true;
			this->ListOfAddresses->ItemHeight = 25;
			this->ListOfAddresses->Location = System::Drawing::Point(12, 104);
			this->ListOfAddresses->Name = L"ListOfAddresses";
			this->ListOfAddresses->Size = System::Drawing::Size(333, 379);
			this->ListOfAddresses->TabIndex = 0;
			this->ListOfAddresses->Sorted = 1;
			this->ListOfAddresses->SelectedIndexChanged += gcnew System::EventHandler(this, &IDS::ListOfAddresses_SelectedIndexChanged);
			// 
			// AddressInput
			// 
			this->AddressInput->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->AddressInput->Location = System::Drawing::Point(12, 515);
			this->AddressInput->Name = L"AddressInput";
			this->AddressInput->Size = System::Drawing::Size(205, 30);
			this->AddressInput->TabIndex = 1;
			// 
			// LoadAddress
			// 
			this->LoadAddress->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->LoadAddress->Location = System::Drawing::Point(228, 501);
			this->LoadAddress->Name = L"LoadAddress";
			this->LoadAddress->Size = System::Drawing::Size(117, 57);
			this->LoadAddress->TabIndex = 2;
			this->LoadAddress->Text = L"Загрузить";
			this->LoadAddress->UseVisualStyleBackColor = true;
			this->LoadAddress->Click += gcnew System::EventHandler(this, &IDS::LoadAddress_Click);
			// 
			// Addresses
			// 
			this->Addresses->AutoSize = true;
			this->Addresses->Font = (gcnew System::Drawing::Font(L"Times New Roman", 19.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->Addresses->Location = System::Drawing::Point(13, 13);
			this->Addresses->Name = L"Addresses";
			this->Addresses->Size = System::Drawing::Size(130, 39);
			this->Addresses->TabIndex = 3;
			this->Addresses->Text = L"Адреса:";
			// 
			// SearchInput
			// 
			this->SearchInput->Font = (gcnew System::Drawing::Font(L"Times New Roman", 13.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->SearchInput->Location = System::Drawing::Point(12, 63);
			this->SearchInput->Name = L"SearchInput";
			this->SearchInput->Size = System::Drawing::Size(333, 33);
			this->SearchInput->TabIndex = 4;
			this->SearchInput->TextChanged += gcnew System::EventHandler(this, &IDS::SearchInput_TextChanged);

			// 
			// ListOfSensors
			// 
			this->ListOfSensors->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ListOfSensors->FormattingEnabled = true;
			this->ListOfSensors->ItemHeight = 22;
			this->ListOfSensors->Location = System::Drawing::Point(362, 104);
			this->ListOfSensors->Name = L"ListOfSensors";
			this->ListOfSensors->Size = System::Drawing::Size(327, 180);
			this->ListOfSensors->Sorted = true;
			this->ListOfSensors->TabIndex = 5;
			this->ListOfSensors->SelectedIndexChanged += gcnew System::EventHandler(this, &IDS::ListOfSensors_SelectedIndexChanged);
			// 
			// LabelPlace
			// 
			this->LabelPlace->AutoSize = true;
			this->LabelPlace->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->LabelPlace->Location = System::Drawing::Point(471, 296);
			this->LabelPlace->Name = L"LabelPlace";
			this->LabelPlace->Size = System::Drawing::Size(140, 22);
			this->LabelPlace->TabIndex = 8;
			this->LabelPlace->Text = L"Расположение:";
			// 
			// PlaceInput
			// 
			this->PlaceInput->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->PlaceInput->Location = System::Drawing::Point(475, 322);
			this->PlaceInput->Name = L"PlaceInput";
			this->PlaceInput->Size = System::Drawing::Size(214, 30);
			this->PlaceInput->TabIndex = 9;
			// 
			// LabelSensors
			// 
			this->LabelSensors->AutoSize = true;
			this->LabelSensors->Font = (gcnew System::Drawing::Font(L"Times New Roman", 19.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->LabelSensors->Location = System::Drawing::Point(356, 13);
			this->LabelSensors->Name = L"LabelSensors";
			this->LabelSensors->Size = System::Drawing::Size(148, 39);
			this->LabelSensors->TabIndex = 10;
			this->LabelSensors->Text = L"Датчики:";
			this->LoadSensor->Click += gcnew System::EventHandler(this, &IDS::LoadSensor_Click);
			// 
			// LoadSensor
			// 
			this->LoadSensor->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->LoadSensor->Location = System::Drawing::Point(403, 368);
			this->LoadSensor->Name = L"LoadSensor";
			this->LoadSensor->Size = System::Drawing::Size(248, 44);
			this->LoadSensor->TabIndex = 11;
			this->LoadSensor->Text = L"Загрузить";
			this->LoadSensor->UseVisualStyleBackColor = true;

			// SensorIDLabel (текст "ID подключенного датчика:")
			this->SensorIDLabel->AutoSize = true;
			this->SensorIDLabel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->SensorIDLabel->Location = System::Drawing::Point(370, 296);
			this->SensorIDLabel->Size = System::Drawing::Size(180, 20);
			this->SensorIDLabel->TabIndex = 6;
			this->SensorIDLabel->Text = L"ID:";

			// CurrentSensorID (сам ID или надпись "Нет...")
			
			this->CurrentSensorID->AutoSize = true;
			this->CurrentSensorID->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->CurrentSensorID->Location = System::Drawing::Point(370, 330);
			this->CurrentSensorID->Size = System::Drawing::Size(150, 20);
			this->CurrentSensorID->TabIndex = 6;
			this->CurrentSensorID->Text = L"Нет";
			// 
			// LabelStatusOfSensor
			// 
			this->LabelStatusOfSensor->AutoSize = true;
			this->LabelStatusOfSensor->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->LabelStatusOfSensor->Location = System::Drawing::Point(362, 434);
			this->LabelStatusOfSensor->Name = L"LabelStatusOfSensor";
			this->LabelStatusOfSensor->Size = System::Drawing::Size(179, 22);
			this->LabelStatusOfSensor->TabIndex = 12;
			this->LabelStatusOfSensor->Text = L"Состояние датчика:";
			// 
			// StatusOfSensor
			// 
			this->StatusOfSensor->AutoSize = true;
			this->StatusOfSensor->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->StatusOfSensor->Location = System::Drawing::Point(362, 471);
			this->StatusOfSensor->Name = L"StatusOfSensor";
			this->StatusOfSensor->Size = System::Drawing::Size(97, 22);
			this->StatusOfSensor->TabIndex = 13;
			this->StatusOfSensor->Text = L"Отключён";
			// 
			// ConnectSensor
			// 
			this->ConnectSensor->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ConnectSensor->Location = System::Drawing::Point(361, 63);
			this->ConnectSensor->Name = L"ConnectSensor";
			this->ConnectSensor->Size = System::Drawing::Size(270, 35);
			this->ConnectSensor->TabIndex = 14;
			this->ConnectSensor->Text = L"Поключить";
			this->ConnectSensor->UseVisualStyleBackColor = true;
			this->ConnectSensor->Click += gcnew System::EventHandler(this, &IDS::ConnectSensor_Click);
			// 
			// IDS
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1153, 601);
			this->Controls->Add(this->ConnectSensor);
			this->Controls->Add(this->StatusOfSensor);
			this->Controls->Add(this->LabelStatusOfSensor);
			this->Controls->Add(this->LoadSensor);
			this->Controls->Add(this->LabelSensors);
			this->Controls->Add(this->PlaceInput);
			this->Controls->Add(this->LabelPlace);
			this->Controls->Add(this->LabelID);
			this->Controls->Add(this->ListOfSensors);
			this->Controls->Add(this->SearchInput);
			this->Controls->Add(this->Addresses);
			this->Controls->Add(this->LoadAddress);
			this->Controls->Add(this->AddressInput);
			this->Controls->Add(this->ListOfAddresses);
			this->Controls->Add(this->SensorIDLabel);
			this->Controls->Add(this->CurrentSensorID);
			this->Name = L"IDS";
			this->Text = L"IDS";
			this->ResumeLayout(false);
			this->PerformLayout();
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &IDS::IDS_FormClosing);

		}

#pragma endregion
};

}
