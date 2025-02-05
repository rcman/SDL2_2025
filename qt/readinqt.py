import sys
from PyQt5.QtWidgets import (QApplication, QWidget, QPushButton,
                             QFileDialog, QVBoxLayout, QLabel, QTextEdit)
from PyQt5.QtCore import Qt

# Assuming animobjects and fshp structures are defined elsewhere.
# For demonstration, I'll use simplified representations.  You'll need
# to adapt this to your actual data structures.

class AnimObject:
    def __init__(self):
        self.max = 0
        self.fshp = []  # List to hold shape data (replace with your actual structure)

class FshpData:  # Placeholder for your actual fshp structure
    def __init__(self):
        self.data = bytearray(272) # Example: 272 bytes of data
        self.flag = 0

animobjects = [AnimObject() for _ in range(10)] # Create 10 AnimObject instances

def read_data_file(filename):
    try:
        with open(filename + ".inf", "r") as inf_file:
            for numrow in range(10):
                animobjects[numrow].max = int(inf_file.readline().strip())
                animobjects[numrow].fshp = [] # Initialize fshp for each row
                for numobj in range(animobjects[numrow].max + 1):
                    fshp_data = FshpData()
                    with open(filename + ".dat", "rb") as dat_file:
                        dat_file.seek(sum(obj.max + 1 for obj in animobjects[:numrow]) * 272 + numobj * 272) #Seek to correct position
                        fshp_data.data = dat_file.read(272)
                    fshp_data.flag = 1
                    animobjects[numrow].fshp.append(fshp_data)

        return True

    except FileNotFoundError:
        return False
    except ValueError:
        return False  # Handle potential errors during int conversion
    except Exception as e: # Catch other potential errors
        print(f"An error occurred: {e}")
        return False

class FileReadWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("File Reader")

        self.file_label = QLabel("No file selected.")
        self.file_text = QTextEdit()  # For displaying file content (optional)
        self.file_text.setReadOnly(True)

        self.open_button = QPushButton("Open File")
        self.open_button.clicked.connect(self.open_file)

        layout = QVBoxLayout()
        layout.addWidget(self.file_label)
        layout.addWidget(self.file_text)  # Optional text display
        layout.addWidget(self.open_button)
        self.setLayout(layout)

    def open_file(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getOpenFileName(self, "Open Data File", "", "All Files (*)", options=options)

        if file_name:
            base_name = file_name.rsplit(".", 1)[0]  # Remove extension
            if read_data_file(base_name):
                self.file_label.setText(f"File: {base_name}.inf and {base_name}.dat read successfully!")
                # Example:  Display some data (adapt as needed)
                # self.file_text.clear()
                # for row in animobjects:
                #     for obj in row.fshp:
                #         self.file_text.append(str(obj.data[:20]))  # Show first 20 bytes
            else:
                self.file_label.setText(f"Error reading {base_name}.inf or {base_name}.dat")
                #self.file_text.setText("Error reading file.")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = FileReadWidget()
    widget.show()
    sys.exit(app.exec_())

