import subprocess
from Tkinter import Tk, RIGHT, BOTH, RAISED, LEFT, TOP, Scale, X, HORIZONTAL, Entry, StringVar, Frame, Button, Label
from ttk import Style
from tkColorChooser import askcolor
from tkFileDialog import askopenfilename, asksaveasfilename
import tkMessageBox
import os
import signal
import pickle
from collections import Counter
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure


__author__ = ("Sambhav Satija", "sambhav13085@iiitd.ac.in")
DEFAULT_WINDOW_GEO = "800x600+300+300"

class UI(Frame):

    def handle_canvas_click_fn(self, type_):
        xmin, xmax = 190, 640
        ymax, ymin = 380, 104
        xrang = xmax - xmin
        yrang = ymax - ymin
        get_xy = lambda x, y: ( min(xrang, max(0, (x - xmin))), min(yrang, max(0, (y - ymin))) )

        single = True

        def trigger_single(event):
            global single
            if single:
                single = False
            else:
                return

            print "Single pressed"
            x, y = get_xy(event.x, event.y)
            print x, y

            n_isovalues = max(self.transfer_fn.iterkeys())
            iso = max(0, min(n_isovalues, int( (float(x) / (xrang)) * n_isovalues + 0.5)))
            alpha = abs(1 - float(y) / (yrang))

            c = askcolor()
            if c is None or c[0] is None:
                return

            self.transfer_fn[iso] = (c[0][0], c[0][1], c[0][2], alpha)
            self.draw_graph()


        def handle_single(event):
            global single
            single = True
            self.parent.after(250, trigger_single, event)


        def handle_double(event):
            global single
            single = False
            print "Double pressed"
            return

        return handle_single if type_ == "single" else handle_double

    def __init__(self, parent, objectfilename, data_count, exec_fn, commit_fn):
        Frame.__init__(self, parent)
        self.parent = parent
        self.transfer_fn = {}
        self.transfer_fn[0] = (0, 0, 0, 0) # set black value
        self.transfer_fn[min(data_count.keys())] = (0, 0, 0, 0) # set black value
        self.transfer_fn[max(data_count.keys())] = (0, 0, 0, 0) # set black value
        self.nodes_text = StringVar()
        self.nodes_text.set("")
        self.initUI(objectfilename, data_count, exec_fn, commit_fn)

    def update_nodes_text(self):
        text = []
        for k, v in sorted(self.transfer_fn.items(), key=lambda x: x[0]):
            text.append("%d %d %d %d %f" % (k, v[0], v[1], v[2], v[3]))
        text = "\n".join(text)
        self.nodes_text.set(text)
        return text

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
        # self.figure_canvas.get_tk_widget

        self.figure_canvas.get_tk_widget().bind("<Double-1>", self.handle_canvas_click_fn("double"))
        self.figure_canvas.get_tk_widget().bind("<Button 1>", self.handle_canvas_click_fn("single"))

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
        self.add_color_button = Button(add_color_panel, text="Set node", command=self.add_color).pack(side=LEFT)
        add_color_panel.pack(after=frame, fill=X)

        Button(add_color_panel, text="Save function", command=self.save_transfer_fn).pack(side=RIGHT)
        Button(add_color_panel, text="Load function", command=self.load_transfer_fn).pack(side=RIGHT)

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

    def load_transfer_fn(self):
        filename = askopenfilename()
        with open(filename, 'r') as f:
            self.transfer_fn = pickle.loads(f.read())
        self.draw_graph()

    def save_transfer_fn(self):
        filename = asksaveasfilename()
        if filename is None: return
        with open(filename, 'w') as f:
            f.write(pickle.dumps(self.transfer_fn))

    def selected_color_fn(self):
        self.selected_color = askcolor()
        self.color_picker.configure(bg=self.selected_color[1])

    def exec_pressed(self):
        self.exec_fn()

    def commit_pressed(self):
        self.commit_fn()

class Instance(Frame):
    EXEC_FILENAME = "./3sat_exec"
    EXEC_TRANSFER_FUNC_COMM = "./3sat_comm_transfer_fn.tf"

    def initUI(self):
        self.root = Tk()
        self.root.resizable(width=False, height=False)
        self.root.geometry(DEFAULT_WINDOW_GEO)
        self.ui = UI(self.root, self.objectfilename, self.data_count, self.start_exec, self.commit)
        self.ui.mainloop()

    def __init__(self):
        self.objectfilename = None

        root = Tk()
        root.withdraw()

        self.objectfilename = "/home/darkryder/Desktop/work/btp/data/NRRD/bonsai.raw"#askopenfilename()
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
        self.exec_proc = subprocess.Popen([Instance.EXEC_FILENAME, filename, Instance.EXEC_TRANSFER_FUNC_COMM])

    def commit(self):
        transfer_fn = self.ui.transfer_fn
        with open(Instance.EXEC_TRANSFER_FUNC_COMM, 'w') as f:
            text = self.ui.update_nodes_text()
            f.write(str(len(text.split("\n"))) + " ") # Write the number of following lines
            f.write(str(max(transfer_fn.keys())) + "\n") # Write number of isovalues
            f.write(text)
        if not (hasattr(self, 'exec_proc') and self.exec_proc and self.exec_proc.poll() is None):
            print "Process is not running."
            return
        os.kill(self.exec_proc.pid, signal.SIGUSR1)

def main():
    instance = Instance()

if __name__ == '__main__':
    main()
