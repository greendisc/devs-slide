#!/usr/bin/python
#
import sys, string

def distance(new_task, server_power_profile):
    dist = abs(new_task[1] - server_power_profile[0])*0.6 + abs(new_task[2] - server_power_profile[1])*0.4
    return dist

def main(argv):
    if len(argv) != 4:
        print "needs 3 arguments: <initial_assignment_file> <new_task_file> <output assignment>"
        exit(1) 
    initial_assignment = {}
    new_task = []
    num_cpus = 0
    '''read current task assignment'''
    f = open(argv[1], 'r')
    for line in f:
        line = line.strip("\n").strip()
        if line == "" or line[0] == '#' or line[0] == ' ' :
            continue
        s = eval(line)
        irc = s[0]
        server = s[1]
        cpu_temp = int(s[3])
        mem_temp = int(s[4])
        #print s, s[0], s[1], s[3], s[4]
        if irc not in initial_assignment:
            initial_assignment[irc] = {}
        initial_assignment[irc][server] = {}
        initial_assignment[irc][server] = (cpu_temp, mem_temp)
        #print initial_assignment[irc][server]
    f.close()
    '''read new task'''
    f = open(argv[2], 'r')
    for line in f:
        line = line.strip("\n").strip()
        if line == "" or line[0] == '#' or line[0] == ' ' :
            continue
        s = eval(line)
        new_task.append(int(s[0]))
        new_task.append(int(s[1]))
        new_task.append(int(s[2]))
        num_cpus = (int(s[0]))
        print "new_task:",new_task
    f.close()
    print "current mapping:"   
    cooler_dist = {}
    cooler_with_empty_slots = {}
    for k,v in initial_assignment.iteritems():
        print k, v
        cooler_dist[k] = 0
    for k,v in initial_assignment.iteritems():    
        current_irc = v
        for s,p in current_irc.iteritems():
            cooler_dist[k] = cooler_dist[k] + distance(new_task, p)
            if p[0] == 0 and p[1] == 0:
                if k in cooler_with_empty_slots:    
                    cooler_with_empty_slots[k].append(s)
                else:
                    cooler_with_empty_slots[k] = []
                    cooler_with_empty_slots[k].append(s)
    print "cooler distances:"
    print cooler_dist
    print "coolers with empty slots:"
    print cooler_with_empty_slots
    min_dist = 100000000
    assigned_cooler = "NONE"
    assigned_server = "NONE"
    for k,v in cooler_dist.iteritems():
        if k in cooler_with_empty_slots and v < min_dist:
            assigned_cooler = k
            assigned_server = cooler_with_empty_slots[k][0]
            min_dist = v
    print "Assigned cooler is: ", assigned_cooler, "assigned server is: ", assigned_server, "with distance: ", min_dist
    f = open(argv[3], 'w')
    f.write("[\"" + assigned_cooler + "\",\"" + assigned_server + "\","
            + str(num_cpus) + "]\n")
    f.close()
if __name__ == "__main__":
    main(sys.argv)
