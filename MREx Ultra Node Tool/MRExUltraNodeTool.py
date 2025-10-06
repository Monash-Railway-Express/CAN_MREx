from PyQt6.QtWidgets import (
    QApplication, QWidget, QLabel, QLineEdit, QComboBox,
    QPushButton, QTextEdit, QVBoxLayout, QHBoxLayout, QGridLayout,
    QTabWidget
)
import sys
import serial
import serial.tools.list_ports
import json
import os
from PyQt6.QtGui import QIcon
from PyQt6.QtCore import Qt


def resource_path(relative_path):
    base_path = getattr(sys, '_MEIPASS', os.path.dirname(__file__))
    return os.path.join(base_path, relative_path)

class TrainProgrammer(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("MREx Ultra Node Tool")
        self.setWindowIcon(QIcon(resource_path("favicon.ico")))
        self.init_ui()
        self.resize(800, 600)  # Width x Height in pixels


    def init_ui(self):
        layout = QVBoxLayout()

        # Tabs
        self.tabs = QTabWidget()
        self.traction_profiles_tab = self.create_traction_profiles_tab()
        self.brake_tab = self.create_brake_tab()
        self.autostop_tab = self.create_autostop_tab()
        self.regen_tab = self.create_regen_tab()
        self.custom_send = self.custom_send_tab()

        self.tabs.addTab(self.traction_profiles_tab, "Traction profiles")
        self.tabs.addTab(self.brake_tab, "Brakes")
        self.tabs.addTab(self.autostop_tab, "Autostop Challenge")
        self.tabs.addTab(self.regen_tab, "Regenerative Braking Challenge")
        self.tabs.addTab(self.custom_send, "Send Custom message")
        layout.addWidget(self.tabs)

        # Serial port selection
        port_layout = QHBoxLayout()
        port_layout.addWidget(QLabel("Serial Port:"))
        self.port_select = QComboBox()
        self.port_select.addItems([port.device for port in serial.tools.list_ports.comports()])
        port_layout.addWidget(self.port_select)
        layout.addLayout(port_layout)

        # Send button
        self.send_btn = QPushButton("Send Configuration")
        self.send_btn.clicked.connect(self.send_config)
        layout.addWidget(self.send_btn)

        # Status log
        self.log = QTextEdit()
        self.log.setReadOnly(True)
        layout.addWidget(self.log)

        self.setLayout(layout)

    def create_traction_profiles_tab(self):
        tab = QWidget()
        grid = QGridLayout()

        grid.addWidget(QLabel(""), 0, 0)
        grid.addWidget(QLabel("Proportional"), 0, 1)
        grid.addWidget(QLabel("Integral"), 0, 2)
        grid.addWidget(QLabel("Derivative"), 0, 3)

        self.traction_inputs = []
        saved_values = self.load_pid_presets()

        for i in range(1, 6):
            grid.addWidget(QLabel(f"Mode {i}:"), i, 0)
            p_input = QLineEdit()
            i_input = QLineEdit()
            d_input = QLineEdit()

            # Load saved values if available
            mode_key = f"Mode{i}"
            if mode_key in saved_values:
                p_input.setText(saved_values[mode_key].get("P", ""))
                i_input.setText(saved_values[mode_key].get("I", ""))
                d_input.setText(saved_values[mode_key].get("D", ""))

            grid.addWidget(p_input, i, 1)
            grid.addWidget(i_input, i, 2)
            grid.addWidget(d_input, i, 3)
            self.traction_inputs.append((p_input, i_input, d_input))

        tab.setLayout(grid)
        return tab


    def create_brake_tab(self):
        tab = QWidget()
        layout = QVBoxLayout()

        row = QHBoxLayout()
        row.addWidget(QLabel("Service Brake Speed:"))

        self.brake_speed = QLineEdit()
        row.addWidget(self.brake_speed)

        layout.addLayout(row)
        tab.setLayout(layout)
        return tab


    def create_autostop_tab(self):
        tab = QWidget()
        grid = QGridLayout()

        grid.addWidget(QLabel("Controller Mode:"), 0, 0)
        self.controller_mode = QComboBox()
        self.controller_mode.addItems(["Manual", "Auto", "Diagnostic"])
        grid.addWidget(self.controller_mode, 0, 1)

        tab.setLayout(grid)
        return tab
    
    def create_regen_tab(self):
        tab = QWidget()
        grid = QGridLayout()

        grid.addWidget(QLabel("Controller Mode:"), 0, 0)
        self.controller_mode = QComboBox()
        self.controller_mode.addItems(["Manual", "Auto", "Diagnostic"])
        grid.addWidget(self.controller_mode, 0, 1)

        tab.setLayout(grid)
        return tab
    
    def custom_send_tab(self):
        tab = QWidget()
        grid = QGridLayout()

        grid.addWidget(QLabel("Node: "), 0, 0)
        self.node = QLineEdit()
        grid.addWidget(self.node, 0,1)

        grid.addWidget(QLabel("Read/write: "), 1, 0)
        self.controller_mode = QComboBox()
        self.controller_mode.addItems(["Read", "Write"])
        grid.addWidget(self.controller_mode, 1, 1)

        grid.addWidget(QLabel("Data: "), 2, 0)
        self.data = QLineEdit()
        grid.addWidget(self.data, 2,1)

        tab.setLayout(grid)
        return tab
    
    def save_pid_presets(self):
        data = {}
        for idx, (p, i, d) in enumerate(self.traction_inputs, start=1):
            data[f"Mode{idx}"] = {
                "P": p.text(),
                "I": i.text(),
                "D": d.text()
            }
        with open("pid_presets.json", "w") as f:
            json.dump(data, f)

    def load_pid_presets(self):
        if os.path.exists("pid_presets.json"):
            with open("pid_presets.json", "r") as f:
                return json.load(f)
        return {}

    def send_config(self):
        port = self.port_select.currentText()
        if not port:
            self.log.append("⚠️ No serial port selected.")
            return

        tab_index = self.tabs.currentIndex()
        message = ""

        if tab_index == 0:  # Traction profiles tab
            self.save_pid_presets()
            traction_data = []
            for idx, (p, i, d) in enumerate(self.traction_inputs, start=1):
                p_val = p.text() or "0"
                i_val = i.text() or "0"
                d_val = d.text() or "0"
                traction_data.append(f"M{idx}:P{p_val},I{i_val},D{d_val}")
            message = "<TRACTION|" + "|".join(traction_data) + ">"

        elif tab_index == 1:  # Brakes tab
            brake = self.brake.currentText()
            message = f"<BRAKE|Profile:{brake}>"

        elif tab_index == 2:  # Controller tab
            mode = self.controller_mode.currentText()
            message = f"<CONTROL|Mode:{mode}>"

        else:
            self.log.append("❌ Unknown tab selected.")
            return

        self.log.append(f"Sending: {message}")

        try:
            with serial.Serial(port, 9600, timeout=1) as ser:
                ser.write(message.encode())
                self.log.append("✅ Sent successfully.")
        except Exception as e:
            self.log.append(f"❌ Error: {e}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = TrainProgrammer()
    window.show()
    sys.exit(app.exec())
