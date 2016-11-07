import subprocess
from Tkinter import Tk, RIGHT, BOTH, RAISED, LEFT, TOP, Scale, X, HORIZONTAL, Entry, StringVar, Frame, Button, Label
from ttk import Style
from tkColorChooser import askcolor
from tkFileDialog import askopenfilename
import tkMessageBox
import os
import signal
from collections import Counter
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure


__author__ = ("Sambhav Satija", "sambhav13085@iiitd.ac.in")
DEFAULT_WINDOW_GEO = "800x600+300+300"

class UI(Frame):
    def __init__(self, parent, objectfilename, data_count, exec_fn, commit_fn):
        Frame.__init__(self, parent)
        self.parent = parent
        self.transfer_fn = {}
        self.transfer_fn[0] = (0, 0, 0, 0) # set black value
        self.nodes_text = StringVar()
        self.nodes_text.set("")
        self.initUI(objectfilename, data_count, exec_fn, commit_fn)

    def update_nodes_text(self):
        text = []
        for k, v in self.transfer_fn.items():
            text.append("%d: %d %d %d %d" % (k, v[0], v[1], v[2], v[3]))
        self.nodes_text.set("\n".join(text))

    def draw_graph(self):
        self.update_nodes_text()
        self.graph.clear()
        self.graph.cla()
        self.graph.bar(self.data_count.keys(), map(lambda x: float(x)/10**5, self.data_count.values()), width=1, color='g')
        items = zip(*sorted(self.transfer_fn.items(), key=lambda x: x[0]))
        self.graph.plot(items[0], map(lambda x: x[3], items[1]), zorder=1)
        self.graph.scatter(items[0], map(lambda x: x[3], items[1]), c=map(lambda x: (x[0]/255.0, x[1]/255.0, x[2]/255.0), items[1]), zorder=2, s=100)
        self.figure_canvas.draw()

    def initUI(self, objectfilename, data_count, exec_fn, commit_fn):
        self.commit_fn = commit_fn
        self.exec_fn = exec_fn
        self.data_count = data_count

        self.parent.title("GPU based Volume Renderer Program Wrapper")
        self.style = Style()
        self.style.theme_use("default")

        frame = Frame(self, relief=RAISED, borderwidth=1)

        self.graph_figure = Figure()
        self.figure_canvas = FigureCanvasTkAgg(self.graph_figure, frame)
        self.graph = self.graph_figure.add_subplot(111)

        self.figure_canvas.show()
        self.figure_canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=True)
        self.figure_canvas._tkcanvas.pack(side=TOP, fill=BOTH, expand=True)
        frame.pack(fill=BOTH, expand=True)

        add_color_panel = Frame(self, borderwidth=1)
        self.isovalue_slider = Scale(add_color_panel, orient=HORIZONTAL, from_=min(data_count.keys()), to=max(data_count.keys()))
        self.isovalue_slider.pack(fill=X, padx=50)
        self.color_picker = Button(add_color_panel, text="Pick Color", command=self.selected_color_fn, bg="white")
        self.color_picker.pack(side=LEFT)
        self.selected_alpha = StringVar(value="0.2")
        self.alpha_text_box = Entry(add_color_panel, text="0.1", textvariable=self.selected_alpha).pack(side=LEFT)
        self.add_color_button = Button(add_color_panel, text="Set node", command=self.add_color).pack(side=RIGHT)
        add_color_panel.pack(after=frame, fill=X)

        self.filename_label = Label(self, text=objectfilename)
        self.filename_label.pack(side=LEFT, padx=5, pady=5)
        self.executeButton = Button(self, text="Run.", command=self.exec_pressed)
        self.executeButton.pack(side=RIGHT, padx=5, pady=5)
        self.commitButton = Button(self, text="Commit.", command=self.commit_pressed)
        self.commitButton.pack(side=RIGHT)
        self.nodes_label = Label(self, textvariable=self.nodes_text)
        self.nodes_label.pack(after=self.commitButton)

        self.draw_graph()
        self.pack(fill=BOTH, expand=True)

    def add_color(self):
        c = self.selected_color[0]
        should_delete = self.selected_alpha.get() == "del"
        if should_delete:
            del self.transfer_fn[self.isovalue_slider.get()]
        else:
            self.transfer_fn[self.isovalue_slider.get()] = (c[0], c[1], c[2], float(self.selected_alpha.get()))
        self.draw_graph()

    def selected_color_fn(self):
        self.selected_color = askcolor()
        self.color_picker.configure(bg=self.selected_color[1])

    def exec_pressed(self):
        self.exec_fn()

    def commit_pressed(self):
        self.commit_fn()

class Instance(Frame):
    EXEC_FILENAME = "./3sat_exec"
    EXEC_SOCKET = "./comm.sock"

    def initUI(self):
        root = Tk()
        root.geometry(DEFAULT_WINDOW_GEO)
        self.ui = UI(root, self.objectfilename, self.data_count, self.start_exec, self.commit)
        self.ui.mainloop()

    def __init__(self):
        self.objectfilename = None

        root = Tk()
        root.withdraw()

        self.objectfilename = askopenfilename()
        self.data_count = self.get_data_with_count()

        root.destroy()
        self.initUI()

    def get_data_with_count(self):
        with open(self.objectfilename, 'rb') as f:
            counter = Counter(map(ord, f.read()))
        del counter[0] # Ignoring blackspace
        return counter

    def start_exec(self):
        if hasattr(self, 'exec_proc') and self.exec_proc and self.exec_proc.poll() is None:
            print "Process already running. Not starting another"
            return
        if self.objectfilename is None:
            raise RuntimeError("Cannot start program with no object file name")
        filename = self.objectfilename.split(os.path.sep)[-1].split(".")[0]
        self.exec_proc = subprocess.Popen([Instance.EXEC_FILENAME, filename])

    def commit(self):
        transfer_fn = self.ui.transfer_fn
        if not (hasattr(self, 'exec_proc') and self.exec_proc and self.exec_proc.poll() is None):
            print "Process is not running."
            return
        os.kill(self.exec_proc.pid, signal.SIGUSR1)

def main():
    instance = Instance()

if __name__ == '__main__':
    main()
