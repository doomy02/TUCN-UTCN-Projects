import os
import subprocess
import threading
from tkinter import Tk, Toplevel, StringVar, Radiobutton, W, Button, messagebox, Text, Scrollbar
from tkinter.font import Font

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.backends._backend_tk import NavigationToolbar2Tk
import matplotlib.pyplot as plt
import customtkinter

# List to store the output
execution_results = []
PATH = r"C:\Users\sebas\OneDrive\Desktop\AN3\Sem1\SSC\proiect\benchmarks"

def execute_and_store():
    execution_results.clear()
    # execute_program(selected_program.get())
    threading.Thread(target=execute_program, args=(selected_program.get(),)).start()


def on_radio_select():
    print(f"Selected program: {selected_program.get()}")


def update_text_field():
    text_field.delete(1.0, "end")
    for language, result in execution_results:
        text_field.insert("end", f"{language}: {result}\n")
    root.after(1000, update_text_field)


def execute_program(program_name):
    print(f"Executing program: {program_name}")
    loading_bar.start()
    new_prog_name = program_name.replace(" ", "")

    # Get the file paths for .cpp, .jar, and .py
    cpp_path = os.path.join(PATH, new_prog_name + ".cpp")
    jar_path = os.path.join(PATH, new_prog_name + ".jar")
    py_path = os.path.join(PATH, new_prog_name + ".py")

    # Check and execute the corresponding file based on the extension
    if os.path.exists(cpp_path):
        compile_command = f"g++ {cpp_path} -o {os.path.splitext(cpp_path)[0]}"
        subprocess.run(compile_command, shell=True)

        run_command = f"{os.path.splitext(cpp_path)[0]}"
        result = subprocess.run(run_command, shell=True, cwd=os.path.dirname(cpp_path), capture_output=True, text=True)
        execution_results.append(("C++", result.stdout.replace("\n", "")))

    if os.path.exists(jar_path):
        run_command = f"java -jar {jar_path}"
        result = subprocess.run(run_command, shell=True, cwd=os.path.dirname(jar_path), capture_output=True, text=True)
        execution_results.append(("Java", result.stdout.replace("\n", "")))

    if os.path.exists(py_path):
        run_command = f"python {py_path}"
        result = subprocess.run(run_command, shell=True, cwd=os.path.dirname(py_path), capture_output=True, text=True)
        execution_results.append(("Python", result.stdout.replace("\n", "")))

    loading_bar.stop()


def show_graph(program_name):
    if not execution_results:
        messagebox.showinfo("Info", "No execution results to display.")
        return

    # Check if the graph window is already open and destroy it
    for widget in root.winfo_children():
        if isinstance(widget, Toplevel):
            widget.destroy()

    # Create a new Toplevel window for the graph
    graph_window = Toplevel(root)
    graph_window.title(f"{program_name}")

    # Unpack the tuples into separate lists for x and y axis
    languages, times = zip(*execution_results)

    # Create a bar graph
    fig, ax = plt.subplots()
    ax.bar(languages, [float(time) for time in times], color=['blue', 'green', 'red'])

    ax.set_xlabel('Programming Languages')
    ax.set_ylabel('Execution Time (s)')
    ax.set_title('Execution Time Comparison')

    canvas = FigureCanvasTkAgg(fig, master=graph_window)
    canvas.draw()
    canvas.get_tk_widget().pack()

    # Toolbar
    toolbar = NavigationToolbar2Tk(canvas, graph_window)
    toolbar.update()
    canvas.get_tk_widget().pack()

    # Display the graph
    graph_window.mainloop()


customtkinter.set_default_color_theme("blue")
customtkinter.set_appearance_mode("dark")

root = customtkinter.CTk()
root.title('Măsurarea timpului de execuție în diferite limbaje de programare')
root.geometry("560x240")
root.resizable(False, False)

program_names = ["Alocare Memorie Dinamica", "Alocare Memorie Statica", "Accesare Memorie Statica",
                 "Accesare Memorie Dinamica", "Creare Thread", "Thread Context Switch",
                 "Thread Migration"]

selected_program = StringVar()

# Frame to hold radio buttons, results text, and scrollbar
frame = customtkinter.CTkFrame(root)
frame.pack(side="left", padx=10)

# Radio buttons
for program_name in program_names:
    customtkinter.CTkRadioButton(root, text=program_name, variable=selected_program, value=program_name,
                                 command=on_radio_select).pack(anchor=W)

# Execute and Store button
execute_button = customtkinter.CTkButton(root, text="Execute", command=lambda: execute_and_store())
execute_button.pack(pady=5)

# Loading bar
loading_bar = customtkinter.CTkProgressBar(root, mode='indeterminate')
loading_bar.pack(pady=5)

# Show Graph button
graph_button = customtkinter.CTkButton(root, text="Show Graph", command=lambda: show_graph(selected_program.get()))
graph_button.pack(pady=5)

# Text field to display execution results
text_field_font = Font(family="Arial", size=14)
text_field = Text(frame, height=10, width=26, wrap="word", font=text_field_font)
text_field.pack(side="left", padx=10)

# Scrollbar for the text field
scrollbar = Scrollbar(frame, command=text_field.yview)
scrollbar.pack(side="right", fill="y")
text_field.config(yscrollcommand=scrollbar.set)

# Update the text field after each execution
update_text_field()

root.after(1000, update_text_field)

root.mainloop()
