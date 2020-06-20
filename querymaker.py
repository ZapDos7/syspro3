#!/usr/bin/env python3

import datetime
import random

start_date = datetime.date(1990, 1, 1)
end_date = datetime.date(2020, 12, 31)

time_between_dates = end_date - start_date
days_between_dates = time_between_dates.days
#random_number_of_days = random.randrange(days_between_dates)
#random_date = start_date + datetime.timedelta(days=random_number_of_days)

commands = ["/diseaseFrequency", "/topk-AgeRanges",
            "/searchPatientRecord", "/numPatientAdmissions", "/numPatientDischarges"]

xwres = ["Argentina", "Australia", "Brazil", "China", "Denmark", "Egypt", "France",
         "Germany", "Greece", "Italy", "Russia", "Switzerland", "Turkey", "USA", "Vietnam"]
file = "client.txt"
astheneies = ["COVID-2019", "SARS-1", "H1N1",
              "FLU-2018", "SARS-COV-2", "MERS-COV", "EVD"]

posa = random.randint(10, 40)  # poses entoles 8a exei to file

f = open(file, "a")

for x in range(posa):
    entoli = random.choice(commands)
    line = entoli
    if entoli == "/diseaseFrequency":
        line = line + " "
        line = line + random.choice(astheneies)
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date1 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date1.strftime("%d-%m-%Y")
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date2 = start_date + datetime.timedelta(days=random_number_of_days)
        while date2 < date1:
            random_number_of_days = random.randrange(days_between_dates)
            date2 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date2.strftime("%d-%m-%Y")
        line = line + " "
        me_xwra = random.randint(1, 2)
        if me_xwra == 1:
            # valta sto file
            line = line + "\n"
            f.write(line)
        else:
            line = line + random.choice(xwres)
            line = line + "\n"
            f.write(line)
    elif entoli == "/topk-AgeRanges":
        line = line + " "
        line = line + str(random.randint(1, 4))
        line = line + " "
        line = line + random.choice(xwres)
        line = line + " "
        line = line + random.choice(astheneies)
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date1 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date1.strftime("%d-%m-%Y")
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date2 = start_date + datetime.timedelta(days=random_number_of_days)
        while date2 < date1:
            random_number_of_days = random.randrange(days_between_dates)
            date2 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date2.strftime("%d-%m-%Y")
        line = line + "\n"
        f.write(line)
    elif entoli == "/searchPatientRecord":
        line = line + " "
        line = line + str(random.randint(100, 9999))
        line = line + "\n"
        f.write(line)
    elif entoli == "/numPatientAdmissions":
        line = line + " "
        line = line + random.choice(astheneies)
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date1 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date1.strftime("%d-%m-%Y")
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date2 = start_date + datetime.timedelta(days=random_number_of_days)
        while date2 < date1:
            random_number_of_days = random.randrange(days_between_dates)
            date2 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date2.strftime("%d-%m-%Y")
        line = line + " "
        me_xwra = random.randint(1, 2)
        if me_xwra == 1:
            line = line + "\n"
            f.write(line)
        else:
            line = line + random.choice(xwres)
            line = line + "\n"
            f.write(line)
    elif entoli == "/numPatientDischarges":
        line = line + " "
        line = line + random.choice(astheneies)
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date1 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date1.strftime("%d-%m-%Y")
        line = line + " "
        random_number_of_days = random.randrange(days_between_dates)
        date2 = start_date + datetime.timedelta(days=random_number_of_days)
        while date2 < date1:
            random_number_of_days = random.randrange(days_between_dates)
            date2 = start_date + datetime.timedelta(days=random_number_of_days)
        line = line + date2.strftime("%d-%m-%Y")
        line = line + " "
        me_xwra = random.randint(1, 2)
        if me_xwra == 1:
            line = line + "\n"
            f.write(line)
        else:
            line = line + random.choice(xwres)
            line = line + "\n"
            f.write(line)
    else:
        pass

# teleiwnei me /exit
f.write("/exit")
#date_time = now.strftime("%m/%d/%Y, %H:%M:%S")
#print("date and time:",date_time)
f.close()
