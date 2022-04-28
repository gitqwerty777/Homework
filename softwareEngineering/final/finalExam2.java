import java.util.HashMap;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Arrays;
import static java.lang.System.out;

class VehicleSystem{
    public HashMap<Integer, Record> records;
    public HashMap<Integer, Vehicle> vehicles;
    public HashMap<Integer, Clerk> clerks;
    public HashMap<Integer, SalesPerson> salesPersons;
    public VehicleSystem(){
	records = new HashMap<Integer, Record>();
	vehicles = new HashMap<Integer, Vehicle>();
	clerks = new HashMap<Integer, Clerk>();
	salesPersons = new HashMap<Integer, SalesPerson>();
    }
    public void addFinalizeRecord(Vehicle v){
	records.put(v.serialNumber, v.record);
    }
    public Record checkRecord(int clerkID, int serialNumber){
	Clerk clerk = clerks.get(clerkID);
	Record record = records.get(serialNumber);
	if(record == null){
	    out.println("invalid serial number");
	} else {
	    clerk.print();
	    vehicles.get(serialNumber).print();
	    record.printWithCost();
	}
	return record;
    }
    public void vehicleArrived(int clerkID, Vehicle v, String[] strs){
	Clerk clerk = clerks.get(clerkID);
	vehicles.put(Integer.parseInt(strs[0]), v);
	clerk.createRecord(v, strs);
	clerk.print();
	v.print();
	v.record.print();
    }
    public void completeSaleInvoice(int salesPersonID, int serialNumber, ArrayList<DealerInstalledOption> options){//TODO: com
	SalesPerson salesPerson = salesPersons.get(salesPersonID);
	Vehicle v = vehicles.get(serialNumber);
	salesPerson.finalizeRecord(v, options);
	salesPerson.print();
	v.printSerial();
	v.record.printDealerOption();
    }
}

class Record{
    public int serialNumber;
    public VehicleInformation info;
    public ArrayList<DealerInstalledOption> options;
    public int cost;
    public int optionCost;
    public Record(VehicleInformation i){
	info = i;
	options = null;
    }
    public void addDealerInstalledOption(ArrayList<DealerInstalledOption> options){
	this.options = options;
	this.calculateCost();
    }
    public void printDealerOption(){
	if(options == null)
	    return;
	out.println("");
	for(int i = 0; i < options.size(); i++){
	    options.get(i).print();
	}
    }
    public void update(int serialNumber, ArrayList<DealerInstalledOption> doption){//to final
	this.serialNumber = serialNumber;
	this.options = doption;
    }
    public int calculateCost(){
	cost = this.info.calculateCost();
	optionCost = 0;
	for(int i = 0; i < options.size(); i++){
	    optionCost += options.get(i).calculateCost();
	}
	cost += optionCost;
	return cost;
    }
    public void print(){
	info.print();
	out.println("");
	printDealerOption();
    }
    public void printWithCost(){
	info.print();
	out.printf(", %d, %d", optionCost, cost);
	printDealerOption();
    }
}

class VehicleInformation{
    public String manufacturer, name, model, year, baseCost, freightCharge;
    public String[] str;
    public VehicleInformation(String[] str){
	this.str = str;
	manufacturer = str[2];
	name = str[3];
	model = str[4];
	year = str[5];
	baseCost = str[6];
	freightCharge = str[7];
    }
    public int calculateCost(){
	return Integer.parseInt(baseCost) + Integer.parseInt(freightCharge);
    }
    public void print(){
	for(int i = 2; i < 7; i++)
	    out.printf("%s, ", str[i]);
	out.printf("%s", str[7]);
    }
}

class DealerInstalledOption{
    public String name;
    public int cost;
    public DealerInstalledOption(String n, int c){
	name = n; cost = c;
    }
    public void print(){
	out.printf("%s-%d\n", name, cost);
    }
    public int calculateCost(){
	return cost;
    }
}

class Vehicle{
    public Vehicle(int serialNumber){
	this.serialNumber = serialNumber;
    }
    public int serialNumber;
    public Record record;
    public String typeName;
    public void printSerial(){
	out.printf("%d", serialNumber);
    }
    public void print(){
	out.printf("%d, %s, ", serialNumber, typeName);
    }
}
class RV extends Vehicle{
    public RV(int serialNumber){
	super(serialNumber);
	typeName = "RV";
    }
}
class Trailer extends Vehicle{
    public Trailer(int serialNumber){
	super(serialNumber);
	typeName = "Trailer";
    }
}

class Staff{
    int id;
    VehicleSystem system;
    public Staff(int id,VehicleSystem system){      
	this.id = id;
	this.system = system;
    }
    public void print(){
	out.printf("%d, ", id);
    }
}
class Clerk extends Staff{
    public Clerk(int id, VehicleSystem system){
	super(id, system);
    }
    public void createRecord(Vehicle v, String[] strs){
	VehicleInformation vinfo = new VehicleInformation(strs);
	v.record = new Record(vinfo);
    }

}
class SalesPerson extends Staff{
    public SalesPerson(int id, VehicleSystem system){
	super(id, system);
    }
    public void finalizeRecord(Vehicle v, ArrayList<DealerInstalledOption> options){
	v.record.addDealerInstalledOption(options);
	system.addFinalizeRecord(v);
    }
}

public class finalExam2 {
    public static void main(String[] args){
	VehicleSystem system = new VehicleSystem();
	Scanner reader = new Scanner(System.in);

	int n_clerk = reader.nextInt();
	for(int i=0; i<n_clerk; i++){
	    int clerkID = reader.nextInt();
	    //System.out.println(clerkID);
	    //TODO store your clerk here
	    system.clerks.put(clerkID, new Clerk(clerkID,system));
	}
		
	int n_salesPerson = reader.nextInt();
	for(int i=0; i<n_salesPerson; i++){
	    int salesPersonID = reader.nextInt();
	    //System.out.println(salesPersonID);
	    //TODO store your salesPerson here
	    system.salesPersons.put(salesPersonID, new SalesPerson(salesPersonID, system));
	}
		
	while(true){
	    String cmd = reader.nextLine();
	    //System.out.println(cmd);
			
	    if(cmd.equals("vehicle_arrived")){
		int clerkID = Integer.parseInt(reader.nextLine());
		String[] tmp = reader.nextLine().split(", ");
		String serialNumber = tmp[0];
		String type = tmp[1];
		String manufacturer = tmp[2];
		String name = tmp[3];
		String model = tmp[4];
		String year = tmp[5];
		String baseCost = tmp[6];
		String freightCharge = tmp[7];
		//System.out.println(clerkID + ", " + serialNumber + ", " + type + ", " + manufacturer  + ", " + name  + ", " + model  + ", " + year  + ", " + baseCost  + ", " + freightCharge);
		//TODO deal with vehicle_arrived here

		int serialNumberi = Integer.parseInt(tmp[0]);
		Vehicle vehicle;
		if(type.equals("RV")){
		    vehicle = new RV(serialNumberi);
		} else {
		    vehicle = new Trailer(serialNumberi);
		}
		Arrays.copyOfRange(tmp, 2, tmp.length);
		system.vehicleArrived(clerkID, vehicle, tmp);
	    }
	    else if(cmd.equals("complete_sales_invoice")){
		int salesPersonID = Integer.parseInt(reader.nextLine());
		int serialNumber = Integer.parseInt(reader.nextLine());
		//System.out.println(salesPersonID + ", " + serialNumber);
		int n_dealerInstalledOptions = Integer.parseInt(reader.nextLine());
		String[] dealerInstalledOptionName = new String[n_dealerInstalledOptions];
		String[] dealerInstalledOptionCost = new String[n_dealerInstalledOptions];
		ArrayList<DealerInstalledOption> options = new ArrayList<DealerInstalledOption>();
		for(int i=0; i<n_dealerInstalledOptions; i++){
		    String[] tmp = reader.nextLine().split(" ");
		    dealerInstalledOptionName[i] = tmp[0];
		    dealerInstalledOptionCost[i] = tmp[1];
		    int cost = Integer.parseInt(tmp[1]);
		    options.add(new DealerInstalledOption(tmp[0], cost));
		    //System.out.println(dealerInstalledOptionName[i] + "-" + dealerInstalledOptionCost[i]);
		}
		//TODO deal with complete_sales_invoice here
		system.completeSaleInvoice(salesPersonID, serialNumber, options);
	    }
	    else if(cmd.equals("finalize_new_vehicle_record")){
		int clerkID = Integer.parseInt(reader.nextLine());
		String serialNumber = reader.nextLine();
		//System.out.println(clerkID + ", " + serialNumber);
		//TODO deal with finalize_new_vehicle_record here
		system.checkRecord(clerkID, Integer.parseInt(serialNumber));
	    }
	    else if(cmd.equals("exit")){
		break;
	    }
	}
    }
}
