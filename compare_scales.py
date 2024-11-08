pid = set()

def compare_files(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        for line1, line2 in zip(f1, f2):
            values1 = [float(x) for x in line1.split()]
            values2 = [float(x) for x in line2.split()]
            if abs(values1[1] - values2[1]) > 0.6:
                pid.add((values1[0],values1[1],values2[1]))
                #print(values1[0],values1[1],values2[1])

file1_path = "brahms_reweighting_test.log"
file2_path = "phenix_reweighting_test.log"

compare_files(file1_path, file2_path)
print(pid)