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

    def handle_canvas_interact_fn(self, type_):
        xmin, xmax = 190, 640
        ymax, ymin = 442, 190
        xrang = xmax - xmin
        yrang = ymax - ymin
        get_xy = lambda x, y: ( min(xrang, max(0, (x - xmin))), min(yrang, max(0, (y - ymin))) )
        get_isovalue_alpha = lambda x, y: (
            max(0, min(n_isovalues, int( (float(x) / (xrang)) * n_isovalues + 0.5))),
            abs(1 - float(y) / (yrang))
        )
        n_isovalues = max(self.transfer_fn.iterkeys())


        self.__ui_inter_single = True
        self.__ui_inter_selected = None

        def trigger_single(event):

            # Check if it is definitely single click
            if self.__ui_inter_single: self.__ui_inter_single = False
            else: return

            iso, alpha = get_isovalue_alpha(*get_xy(event.x, event.y))

            # Present either old color or new default color
            if self.__ui_inter_selected is not None:
                default_color=self.transfer_fn[self.__ui_inter_selected]
                c = askcolor(color=(default_color[0], default_color[1], default_color[2]))
            else:
                c = askcolor()

            if c is None or c[0] is None: return

            # delete selected node if exists.
            if self.__ui_inter_selected:
                del self.transfer_fn[self.__ui_inter_selected]
                self.__ui_inter_selected = None

            # Add this new node to the transfer function
            self.transfer_fn[iso] = (c[0][0], c[0][1], c[0][2], alpha)
            self.draw_graph()

        def handle_single(event):
            self.__ui_inter_single = True
            self.parent.after(250, trigger_single, event)

        def handle_double(event):
            self.__ui_inter_single = False
            iso, alpha = get_isovalue_alpha(*get_xy(event.x, event.y))

            # double click after selecting a node means delete it
            if self.__ui_inter_selected:
                del self.transfer_fn[self.__ui_inter_selected]
                self.__ui_inter_selected = None
                self.draw_graph()
                return

            # Find nearby nodes
            consider = []
            for k, v in self.transfer_fn.iteritems():
                if abs(k - iso) < 20 and abs(v[3] - alpha) < 0.3:
                    consider.append(k)
            if not consider:
                return

            # Select the nearest node
            consider.sort( key = lambda k: abs(self.transfer_fn[k][3]*10 - alpha*10) + abs(k-iso) )
            self.__ui_inter_selected = consider[0]

            self.draw_graph(selected_node_iso=self.__ui_inter_selected)

        def handle_motion(event):
            if self.__ui_inter_selected is None: return
            iso, alpha = get_isovalue_alpha(*get_xy(event.x, event.y))
            self.draw_graph(selected_node_iso=self.__ui_inter_selected, misc_items=[iso, alpha])

        return {
            "single": handle_single,
            "double": handle_double,
            "motion": handle_motion
        }[type_]

    def __init__(self, parent, objectfilename, data_count, exec_fn, commit_fn):
        Frame.__init__(self, parent)
        self.parent = parent
        self.transfer_fn = {}
        self.transfer_fn[0] = (0, 0, 0, 0) # set black value
        self.transfer_fn[min(data_count.keys())] = (0, 0, 0, 0) # set black value
        self.transfer_fn[max(data_count.keys())] = (0, 0, 0, 0) # set black value
        self.initUI(objectfilename, data_count, exec_fn, commit_fn)

    def draw_graph(self, selected_node_iso = None, misc_items=[]):
        self.graph.clear()
        self.graph.cla()
        self.graph.bar(self.data_count.keys(), map(lambda x: float(x)/10**5, self.data_count.values()), width=1, color='g')
        items = zip(*sorted(self.transfer_fn.items(), key=lambda x: x[0]))
        self.graph.plot(items[0], map(lambda x: x[3], items[1]), zorder=1)

        s = 100
        if selected_node_iso is not None:
            idx = items[0].index(selected_node_iso)
            s = [100 for _ in xrange(len(items[0]))]
            s[idx] = 200

        self.graph.scatter(items[0], map(lambda x: x[3], items[1]), c=map(lambda x: (x[0]/255.0, x[1]/255.0, x[2]/255.0), items[1]), zorder=2, s=s)
        if misc_items:
            self.graph.scatter(misc_items[0], misc_items[1], s=200, zorder=3)

        self.figure_canvas.draw()


    def update_nodes_text(self):
        text = []
        for k, v in sorted(self.transfer_fn.items(), key=lambda x: x[0]):
            text.append("%d %d %d %d %f" % (k, v[0], v[1], v[2], v[3]))
        text = "\n".join(text)
        return text

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

        self.figure_canvas.get_tk_widget().bind("<Double-1>", self.handle_canvas_interact_fn("double"))
        self.figure_canvas.get_tk_widget().bind("<Button 1>", self.handle_canvas_interact_fn("single"))
        self.figure_canvas.get_tk_widget().bind("<Motion>", self.handle_canvas_interact_fn("motion"))

        self.graph = self.graph_figure.add_subplot(111)

        self.figure_canvas.show()
        self.figure_canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=True)
        self.figure_canvas._tkcanvas.pack(side=TOP, fill=BOTH, expand=True)
        frame.pack(fill=BOTH, expand=True)

        add_color_panel = Frame(self, borderwidth=1)
        add_color_panel.pack(after=frame, fill=X)


        self.filename_label = Label(self, text=objectfilename)
        self.executeButton = Button(self, text="Run", command=self.exec_pressed)
        self.commitButton = Button(self, text="Commit", command=self.commit_pressed)

        self.filename_label.pack(side=LEFT, padx=5, pady=5)
        Button(self, text="Save function", command=self.save_transfer_fn).pack(side=RIGHT, padx=5, pady=5)
        Button(self, text="Load function", command=self.load_transfer_fn).pack(side=RIGHT, padx=5, pady=5)
        self.executeButton.pack(side=RIGHT, padx=5, pady=5)
        self.commitButton.pack(side=RIGHT)

        self.draw_graph()
        self.pack(fill=BOTH, expand=True)

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
