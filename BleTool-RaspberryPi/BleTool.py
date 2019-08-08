import tkinter as tk
import tkinter.font as tkFont
import threading
import time
from tkinter.messagebox import *
from tkinter import ttk
from bluepy.btle import Scanner
from ImageLabel import ImageLabel
from bluepy.btle import Peripheral, UUID


devices = None

class MainApplication(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        tk.Frame.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.center_window(kwargs['width'], kwargs['height'])
        self.set_title('BleTool')
        self.set_unresizable()
        
        '''
            widgets
        '''
        self.font = tkFont.Font(family='Times New Roman', size=15)
        self.go = tk.Button(self.parent, text='search', font=self.font, relief=tk.GROOVE, command=self.call_search)
        self.go.place(x=290, y=250)
        self.animation = ImageLabel(self.parent, bd=0)
        self.animation.load('./LoadAnimation/load_1.gif')
        # set the bold style: weight = tk.Font.BOLD
        self.search_box = tk.Entry(self, font=self.font, relief=tk.RIDGE)
        self.search_box.place(x=60, y=250, width=210, height=40)

    def set_unresizable(self, width=False, height=False):
        root = self.parent
        root.resizable(width, height)

    def set_title(self, title):
        root = self.parent
        root.title(title)

    def center_window(self, width, height):
        root = self.parent
        screenwidth = root.winfo_screenwidth()
        screenheight = root.winfo_screenheight()
        size = '%dx%d+%d+%d' % (width, height, (screenwidth-width)/2, (screenheight-height)/2)
        root.geometry(size)

    def scan_btle_devices(self):
        # p = Peripheral('DE:5B:BA:87:4C:F7', 'random')
        # chList = p.getCharacteristics()
        # for ch in chList():
        time.sleep(10)
        self.animation.place_forget()
        self.go.place_forget()
        self.search_box.place_forget()

    def call_search(self):
        address = self.search_box.get()
        if len(address)==17 and len(address.split(':'))==6:
            self.animation.place(x=0, y=60)
            t = threading.Thread(target=self.scan_btle_devices)
            t.start()
        else:
            showerror(title='Warning', message='Please input the device address with correct pattern(XX:XX:XX:XX:XX:XX)')

if __name__ == "__main__":
    root = tk.Tk()
    MainApplication(root, height=647, width=403, bg='white').pack(side='top', fill='both', expand=True)
    root.mainloop()
