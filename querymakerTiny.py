#!/usr/bin/env python3

import datetime
import random
import string

def get_random_alphaNumeric_string(stringLength=5):
    lettersAndDigits = string.ascii_letters.lower() + string.digits
    return ''.join((random.choice(lettersAndDigits) for i in range(stringLength)))

start_date = datetime.date(1990, 1, 1)
end_date = datetime.date(2020, 12, 31)

time_between_dates = end_date - start_date
days_between_dates = time_between_dates.days
#random_number_of_days = random.randrange(days_between_dates)
#random_date = start_date + datetime.timedelta(days=random_number_of_days)

commands = ["/diseaseFrequency", "/topk-AgeRanges",
            "/searchPatientRecord", "/numPatientAdmissions", "/numPatientDischarges"]

xwres = ["India", "Liberia", "Russia", "Sudan", "Ukraine", "Vietnam"]
file = "clientTiny.txt"
astheneies = ["AIDS", "Babesiosis", "COVID-19", "Diabetes", "Echinococcosis", "Flu", "Gout", "H1N1", "Injury", "Jaundice", "Kala-Azar", "Leprosy", "Malaria", "Neurocysticercosis", "Obesity", "Plague", "Q-Fever", "Rabies", "Salmonella", "Tapeworm", "URDO"]

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
        #line = line + str(random.randint(100, 9999))
        #id="$(random-string 5)"
        id_generated = get_random_alphaNumeric_string()
        line = line + id_generated
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
