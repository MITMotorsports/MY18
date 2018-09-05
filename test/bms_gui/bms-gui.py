import tkinter as tk
from tkinter import StringVar, IntVar, DoubleVar
import threading

class GUI:
    def __init__(self, msgs):
        '''
        make bms table with the labels in stat_names
        '''
        self.root = tk.Tk()
        self.root.title = "MY18 BMS"
        self.table = BMS_TABLE(msgs, self.root)

    def update_table(self, can_msg):
        '''
        call in a loop to update bms tabnle values
        takes in can_msg as dict
        '''

        self.table.update(can_msg)

    def update(self):
        '''
        call in loop to update gui for new can msg vals and fault button press
        '''
        self.root.update_idletasks()
        self.root.update()


class BMS_TABLE():

    MAX_ROWS = 4
    PAD_X = 10
    PAD_Y = 5
    MESSAGES_PER_ROW = 2

    def __init__(self, msgs, root):
        '''
        makes grid entries for can msg segment names and their values
        '''
        self.msg_vals = {}

        last_row = self.make_rows(msgs, root)
        self.make_fault_button(last_row, root)

        root.update_idletasks()
        root.update()

    def update(self,can_dict):
        '''
        udate can msg values
        '''
        can_msg = can_dict['can0']
        name = list(can_msg.keys())[0]

        for sgmt_name, val in can_msg[name].items():
            self.msg_vals[name+"_"+sgmt_name].set(str(val))
        print(self.msg_vals)

    def make_rows(self,msg_names, root):
        '''
        makes grid labels with text corresponding to can msg names
        makes grid labels with can msg value variables that can be updated
        '''
        
        curr_title_row = 0
        curr_col = 0
        curr_row = 1

        num_msgs = 1

        for msg_name, sgmts in msg_names.items():

            if(num_msgs > 2):
                num_msgs = 1
                curr_title_row = self.MAX_ROWS + 1
                curr_col = 0


            tk.Label(root, text = msg_name, font = ('Arial', 14)).grid(row = curr_title_row, column = curr_col, pady = self.PAD_Y)
            num_msgs += 1
            for sgmt in sgmts:

                if curr_row > self.MAX_ROWS:
                    curr_row = 1
                    curr_col += 2

                tk.Label(root, text = sgmt, font = ('Arial', 12)).grid(row = curr_row + curr_title_row, column = curr_col, pady = self.PAD_Y)
                #make can msg value label
                self.msg_vals[msg_name+"_"+sgmt] = StringVar()
                #self.msg_vals[msg_name+"_"+sgmt].set("0.0")
                tk.Label(root, textvariable = self.msg_vals[msg_name+"_"+sgmt], font = ('Arial', 12)).grid(row = curr_row + curr_title_row, column = curr_col + 1, padx = self.PAD_X, pady = self.PAD_Y)
                curr_row+=1

        return curr_row + curr_title_row

    def make_fault_button(self, last_row, root):
        '''
        create fault button
        '''


        b = tk.Button(root, text = "FAULT", font = ('Arial', 18), command = self.send_fault).grid(row=last_row + 1, column=0)

    def send_fault(self):
        '''
        fault button callback function
        sends can msg to bms to fault
        '''
        pass


msgs = {'CellTemperatureRange': ['min', 'argmin', 'max0', 'argmax0', 'max1', 'argmax1', 'max2'], 'CellTEmperatureVariance': ['mean', 'variance'], 'CellVoltageRange':['min', 'max', 'sum']}
gui = GUI(msgs)



#loop = threading.Thread(target = gui.update({'can0': {'CellTemperatureRange': {'min': '183', 'argmin': '63', 'max0': '199', 'argmax0': '70', 'max1': '198', 'argmax1': '55', 'max2': '197'}}})).start()
#root.mainloop()

ticks = 0
while(True):
    gui.update()








