import java.util.HashMap;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.HashSet;
import java.util.Scanner;
import static java.lang.System.out;

class Book{
    public int bid;
    public String author, subject;
    public boolean isBorrowed;
    public User lastestBorrowedUser;
    public Book(int bid, String author, String subject){
	this.bid = bid;
	this.author = author;
	this.subject = subject;
	isBorrowed = false;
	lastestBorrowedUser = null;
    }

    public boolean checkout(User u){
	if(isBorrowed)
	    return false;
	else{
	    isBorrowed = true;
	    lastestBorrowedUser = u;
	    return true;
	}
    }
    public boolean returnBook(User u){
	if(!isBorrowed || lastestBorrowedUser == null || u.uid != lastestBorrowedUser.uid)
	    return false;
	else{
	    isBorrowed = false;
	    return true;
	}
    }
}

abstract class User{
    public int uid;
    public Library library;
    public int borrowN;

    public User(int uid, Library library){
	this.uid = uid;
	this.library = library;
	this.borrowN = 0;
    }
    
    public abstract boolean reachMaxBorrow();
    public void checkout(int uid, int bid){
	out.println("fail");
    }
    public void returnBook(int uid, int bid){
	out.println("fail");
    }
    public void addBook(int bid, String author, String subject){
	out.println("fail");
    }
    public void removeBook(int bid){
	out.println("fail");
    }
    public void checkList(int uid){
	out.println("fail");
    }
    public void lastChecked(int bid){
	out.println("fail");
    }
    public void list_author(String author){
	out.println("fail");
    }
    public void list_subject(String subject){
	out.println("fail");
    }
}
class Borrower extends User{
    public int maxborrown;

    public Borrower(int uid, int maxborrown, Library library){
	super(uid, library);
	this.maxborrown = maxborrown;
    }

    public boolean reachMaxBorrow(){
	return borrowN >= maxborrown;
    }
    public void checkList(int uid){
	if(uid != this.uid)
	    out.println("fail");
	else
	    library.checkList(uid);
    }
    public void list_author(String author){
	library.list_author(author);
    }
    public void list_subject(String subject){
	library.list_subject(subject);
    }
}
class Staff extends User{
    public Staff(int uid, Library library){
	super(uid, library);
    }

    public boolean reachMaxBorrow(){
	return false;
    }
    public void checkout(int uid, int bid){
	library.checkout(uid, bid);
    }
    public void returnBook(int uid, int bid){
	library.returnBook(uid, bid);
    }
    public void addBook(int bid, String author, String subject){
	library.addBook(bid, author, subject);
    }
    public void removeBook(int bid){
	library.removeBook(bid);
    }
    public void checkList(int uid){
	library.checkList(uid);
    }
    public void lastChecked(int bid){
	library.lastChecked(bid);
    }
    public void list_author(String author){
	library.list_author(author);
    }
    public void list_subject(String subject){
	library.list_subject(subject);
    }
}

class Library{
    public HashSet<Book> booklist;
    public HashSet<User> userlist;
    public HashMap<Integer, Book> bookmap;
    public HashMap<Integer, User> usermap;
    public User nowUser;

    public Library(Scanner reader){
	booklist = new HashSet<Book>();
	userlist = new HashSet<User>();
	bookmap = new HashMap<Integer, Book>();
	usermap = new HashMap<Integer, User>();
	readInitData(reader);
    }

    public void readInitData(Scanner reader){
	int bookn = reader.nextInt();
	for(int i = 0; i < bookn; i++){
	    int id = reader.nextInt();
	    String author = reader.next();
	    String subject = reader.next();
	    Book b = new Book(id, author, subject);
	    booklist.add(b);
	    bookmap.put(id, b);
	}
	int usern = reader.nextInt();
	for(int i = 0; i < usern; i++){
	    String type = reader.next();
	    int id = reader.nextInt();
	    if(type.equals("borrower")){
		int maxborrown = reader.nextInt();
		User u = new Borrower(id, maxborrown, this);
		userlist.add(u);
		usermap.put(id, u);
	    } else {
		User u = new Staff(id, this);
		userlist.add(u);
		usermap.put(id, u);
	    }
	}
    }
    
    public void checkout(int uid, int bid){
	//out.print("checkout:");
	Book b = bookmap.get(bid);
	User u = usermap.get(uid);
	if(!u.reachMaxBorrow() && b.checkout(u)){
	    out.println("success");
	    u.borrowN++;
	} else {
	    out.println("fail");
	}
    }
    public void returnBook(int uid, int bid){
	//out.print("returnbook:");
	User u = usermap.get(uid);
	Book b = bookmap.get(bid);
	if(b.returnBook(u)){
	    out.println("success");
	    u.borrowN--;
	}
	else
	    out.println("fail");
	out.println("success");
    }
    public void addBook(int id, String author, String subject){
	//out.print("addbook:");
	if(bookmap.get(id) != null){//book exist
	    out.println("fail");
	} else {
	    Book b = new Book(id, author, subject);
	    booklist.add(b);
	    bookmap.put(id, b);
	    out.println("success");
	}
    }
    public void removeBook(int id){
	//out.print("removebook:");
	Book toremove = bookmap.get(id);
	if(toremove == null){
	    out.println("fail");
	} else {
	    bookmap.remove(toremove);
	    booklist.remove(toremove);
	    out.println("success");
	}
    }
    public void checkList(int uid){
	//out.print("checkList:");
	Iterator<Book> it = booklist.iterator();
	boolean isFirst = true;
	while(it.hasNext()) {
	    Book b = (Book)it.next();
	    if(b.lastestBorrowedUser != null && b.lastestBorrowedUser.uid == uid){
		if(isFirst){//TOTALLY WASTE MY TIME
		    out.printf("%d", b.bid);
		    isFirst = false;
		}
		else
		    out.printf(",%d", b.bid);
	    }
	}
	out.println("");
    }
    public void lastChecked(int bid){
	Book b = bookmap.get(bid);
	if(b == null){
	    out.println("fail");
	    return;
	} else if(b.lastestBorrowedUser == null){
	    out.println("");
	    return;
	} else {
	    out.printf("%d\n", b.lastestBorrowedUser.uid);
	}
    }

    public void list_author(String author){
	//out.print("list author:");
	Iterator<Book> it = booklist.iterator();
	boolean isFirst = true;
	while(it.hasNext()) {
	    Book b = (Book)it.next();
	    if(b.author.equals(author))
		if(isFirst){
		    out.printf("%d", b.bid);
		    isFirst = false;
		}
		else
		    out.printf(",%d", b.bid);
	}
	out.println("");
    }
    public void list_subject(String subject){
	//out.print("list subject:");
	Iterator<Book> it = booklist.iterator();
	boolean isFirst = true;
	while(it.hasNext()) {
	    Book b = (Book)it.next();
	    if(b.subject.equals(subject))
		if(isFirst){
		    out.printf("%d", b.bid);
		    isFirst = false;
		}
		else
		    out.printf(",%d", b.bid);
	}
	out.println("");
    }

    public void readCommand(Scanner reader){
	while(true){
	    String nowuid = reader.next();
	    if(nowuid.equals("exit"))
		break;
	    int intid = Integer.parseInt(nowuid);
	    //out.printf("now is user %d\n", intid);
	    nowUser = usermap.get(intid);

	    String command = reader.next();
	    //out.printf("do %s\n", command);

	    int uid, bid;
	    String author, subject, secondaryCommand, argument;

	    switch(command){
	    case "checkout":
		uid = reader.nextInt();
		bid = reader.nextInt();
		nowUser.checkout(uid, bid);
		break;
	    case "return":
		uid = reader.nextInt();
		bid = reader.nextInt();
		nowUser.returnBook(uid, bid);
		break;
	    case "add":
		bid = reader.nextInt();
		author = reader.next();
		subject = reader.next();
		nowUser.addBook(bid, author, subject);
		break;
	    case "remove":
		bid = reader.nextInt();
		author = reader.next();
		subject = reader.next();
		nowUser.removeBook(bid);
		break;
	    case "list":
		secondaryCommand = reader.next();
		argument = reader.next();
		if(secondaryCommand.equals("author")){
		    nowUser.list_author(argument);
		} else if(secondaryCommand.equals("subject")){
		    nowUser.list_subject(argument);
		} else {
		    out.println("fail");
		}
		break;
	    case "checked-list":
		uid = reader.nextInt();
		nowUser.checkList(uid);
		break;
	    case "last-checked":
		bid = reader.nextInt();
		nowUser.lastChecked(bid);
		break;
	    }
	}
    }
}

public class Quiz{
    public static Scanner reader = new Scanner(System.in);
    public static Library library = new Library(reader);
    public static void main(String[] args){
	library.readCommand(reader);
    }
}
