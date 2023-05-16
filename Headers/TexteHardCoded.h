#include <string>
#include <map>
typedef std::map <std::string,std::vector<std::string>> PromptMap;
namespace HardcodeRayJump
{
const std::string HELP_MESSEAGE = R"(Welcome to RayJump (also known as Ray-cer Typer)

Objective:
    Type out the given text as fast as you can.

Mechanics:
    Type the first letter to begin a new race.
    Correctly typed characters will turn green, while incorrect ones will turn red.
    The game ends when the player types out all the words in the list and presses Enter.
    To go back to the game press Esc.
    Right Click on a car to change its color;
    Press Ctrl + C to copy and Ctrl + V to set your own text.
    You only need to use the english alphabet even for Romanian diacritics.
    Hold the escape key to quit the game.

Typing Tips:
    Sit with your back straight and your feet flat on the ground.
    Rest your index fingers on the home keys (F and J);
    Type with a smooth and steady rhythm.
    Practice typing regularly to improve your speed and accuracy.)";

const std::string HELP_MESSEAGE_RO = R"(Bine ați venit la RayJump (cunoscut și sub numele de Ray-cer Typer)

Obiectiv:
    Scrieți textul dat cât de repede puteți.

Mecanică:
    Tastați prima literă pentru a începe o nouă cursă.
    Caracterele tastate corect vor deveni verzi, în timp ce cele incorecte vor deveni roșii.
    Jocul se termină atunci când jucătorul tastează toate cuvintele din listă și apasă Enter.
    Pentru a vă întoarce la joc apăsați Esc.
    Faceți clic dreapta pe o mașină pentru a-i schimba culoarea;
    Apăsați Ctrl + C pentru a copia și Ctrl + V pentru a seta propriul text.
    Trebuie să folosiți doar alfabetul englezesc, chiar și pentru diacriticele românești.
    Țineți apăsat pe tasta Escape pentru a ieși din joc.

Sfaturi de dactilografiere:
    Stați cu spatele drept și cu picioarele pe pământ.
    Sprijiniți-vă degetele arătătoare pe tastele de pornire (F și J);
    Tastați cu un ritm lin și constant.
    Exersați dactilografierea în mod regulat pentru a vă îmbunătăți viteza și acuratețea.)";

const PromptMap PROMPTS_BY_LANGUAGE =
{
    {
        "english",{
            "I failed my way to success. - Thomas Edison",
            "Dream big and dare to fail. - Norman Vaughan",
            "You become what you believe. - Oprah Winfrey",
            "An unexamined life is not worth living. - Socrates",
            "Life is a long lesson in humility. - James M. Barrie",
            "The purpose of our lives is to be happy. - Dalai Lama",
            "May you live all the days of your life. - Jonathan Swift",
            "Life is trying things to see if they work. - Ray Bradbury",
            "Whoever is happy will make others happy too. - Anne Frank",
            "You miss 100% of the shots you don't take. - Wayne Gretzky",
            "Life is ours to be spent, not to be saved. - D. H. Lawrence",
            "Love the life you live. Live the life you love. - Bob Marley",
            "Dreaming, after all, is a form of planning. - Gloria Steinem",
            "Believe you can and you're halfway there. - Theodore Roosevelt",
            "I never dreamed about success, I worked for it. - Estee Lauder",
            "I would rather die of passion than of boredom. - Vincent van Gogh",
            "Winning isn't everything, but wanting to win is. - Vince Lombardi",
            "Life is either a daring adventure or nothing at all. - Helen Keller",
            "Only a life lived for others is a life worthwhile. - Albert Einstein",
            "You only live once, but if you do it right, once is enough. - Mae West",
            "The only impossible journey is the one you never begin. - Tony Robbins",
            "Life itself is the most wonderful fairy tale. - Hans Christian Andersen",
            "Life is what happens when you're busy making other plans. - John Lennon",
            "The way to get started is to quit talking and begin doing. - Walt Disney",
            "Do not let making a living prevent you from making a life. - John Wooden",
            "Life is made of ever so many partings welded together. - Charles Dickens",
            "Life is really simple, but we insist on making it complicated. - Confucius",
            "Everything you've ever wanted is on the other side of fear. - George Addair",
            "Whether you think you can or you think you can't, you're right. - Henry Ford",
            "It does not matter how slowly you go as long as you do not stop. - Confucius",
            "Live in the sunshine, swim the sea, drink the wild air. - Ralph Waldo Emerson",
            "Don't be afraid to give up the good to go for the great. - John D. Rockefeller",
            "Never let the fear of striking out keep you from playing the game. - Babe Ruth",
            "Nothing is impossible, the word itself says, ‘I'm possible!' - Audrey Hepburn",
            "I find that the harder I work, the more luck I seem to have. - Thomas Jefferson",
            "Life is 10% what happens to me and 90% of how I react to it. - Charles Swindoll",
            "Before anything else, preparation is the key to success. - Alexander Graham Bell",
            "It is during our darkest moments that we must focus to see the light. - Aristotle",
            "I didn't fail the test. I just found 100 ways to do it wrong. - Benjamin Franklin",
            "Whatever the mind of man can conceive and believe, it can achieve. - Napoleon Hill",
            "The question isn't who is going to let me; it's who is going to stop me. - Ayn Rand",
            "If you really look closely, most overnight successes took a long time. - Steve Jobs",
            "It is better to fail in originality than to succeed in imitation. - Herman Melville",
            "Try not to become a man of success. Rather become a man of value. - Albert Einstein",
            "The only place where success comes before work is in the dictionary. - Vidal Sassoon",
            "Too many of us are not living our dreams because we are living our fears. - Les Brown",
            "Things work out best for those who make the best of how things work out. - John Wooden",
            "You will face many defeats in life, but never let yourself be defeated. - Maya Angelou",
            "I attribute my success to this: I never gave or took any excuse. - Florence Nightingale",
            "Always remember that you are absolutely unique. Just like everyone else. - Margaret Mead",
            "You may be disappointed if you fail, but you are doomed if you don't try. - Beverly Sills",
            "The road to success and the road to failure are almost exactly the same. - Colin R. Davis",
            "Tell me and I forget. Teach me and I remember. Involve me and I learn. - Benjamin Franklin",
            "The secret of success is to do the common thing uncommonly well. - John D. Rockefeller Jr.",
            "In three words I can sum up everything I've learned about life: it goes on. - Robert Frost",
            "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
            "Success is walking from failure to failure with no loss of enthusiasm. - Winston Churchill",
            "When you reach the end of your rope, tie a knot in it and hang on. - Franklin D. Roosevelt",
            "Success usually comes to those who are too busy to be looking for it. - Henry David Thoreau",
            "Life is a succession of lessons which must be lived to be understood. - Ralph Waldo Emerson",
            "If you are not willing to risk the usual, you will have to settle for the ordinary. - Jim Rohn",
            "The most difficult thing is the decision to act, the rest is merely tenacity. - Amelia Earhart",
            "If you're offered a seat on a rocket ship, don't ask what seat! Just get on. - Sheryl Sandberg",
            "If life were predictable it would cease to be life, and be without flavor. - Eleanor Roosevelt",
            "Life is never fair, and perhaps it is a good thing for most of us that it is not. - Oscar Wilde",
            "The only person you are destined to become is the person you decide to be. - Ralph Waldo Emerson",
            "I have learned over the years that when one's mind is made up, this diminishes fear. - Rosa Parks",
            "The only limit to our realization of tomorrow will be our doubts of today. - Franklin D. Roosevelt",
            "Spread love everywhere you go. Let no one ever come to you without leaving happier. - Mother Teresa",
            "In this life we cannot do great things. We can only do small things with great love. - Mother Teresa",
            "Go confidently in the direction of your dreams! Live the life you've imagined. - Henry David Thoreau",
            "Keep smiling, because life is a beautiful thing and there's so much to smile about. - Marilyn Monroe",
            "If you genuinely want something, don't wait for it - teach yourself to be impatient. - Gurbaksh Chahal",
            "Don't judge each day by the harvest you reap but by the seeds that you plant. - Robert Louis Stevenson",
            "In the end, it's not the years in your life that count. It's the life in your years. - Abraham Lincoln",
            "You know you are on the road to success if you would do your job and not be paid for it. - Oprah Winfrey",
            "The greatest glory in living lies not in never falling, but in rising every time we fall. - Nelson Mandela",
            "How wonderful it is that nobody need wait a single moment before starting to improve the world. - Anne Frank",
            "Do not go where the path may lead, go instead where there is no path and leave a trail. - Ralph Waldo Emerson",
            "Success is not final; failure is not fatal: It is the courage to continue that counts. - Winston S. Churchill",
            "I alone cannot change the world, but I can cast a stone across the water to create many ripples. - Mother Teresa",
            "A successful man is one who can lay a firm foundation with the bricks others have thrown at him. - David Brinkley",
            "Always bear in mind that your own resolution to success is more important than any other one thing. - Abraham Lincoln",
            "There are no secrets to success. It is the result of preparation, hard work, and learning from failure. - Colin Powell",
            "You have brains in your head. You have feet in your shoes. You can steer yourself any direction you choose. - Dr. Seuss",
            "If you set your goals ridiculously high and it's a failure, you will fail above everyone else's success. - James Cameron",
            "Many of life's failures are people who did not realize how close they were to success when they gave up. - Thomas A. Edison",
            "When everything seems to be going against you, remember that the airplane takes off against the wind, not with it. - Henry Ford",
            "Successful people do what unsuccessful people are not willing to do. Don't wish it were easier; wish you were better. - Jim Rohn",
            "Success seems to be connected with action. Successful people keep moving. They make mistakes but they don't quit. - Conrad Hilton",
            "The best and most beautiful things in the world cannot be seen or even touched - they must be felt with the heart. - Helen Keller",
            "Don't be distracted by criticism. Remember - the only taste of success some people get is to take a bite out of you. - Zig Ziglar",
            "If you want to achieve excellence, you can get there today. As of this second, quit doing less-than-excellent work. - Thomas J. Watson",
            "If you look at what you have in life, you'll always have more. If you look at what you don't have in life, you'll never have enough. - Oprah Winfrey",
            "I've learned that people will forget what you said, people will forget what you did, but people will never forget how you made them feel. - Maya Angelou",
            "Your time is limited, so don't waste it living someone else's life. Don't be trapped by dogma - which is living with the results of other people's thinking. - Steve Jobs",
            "The real test is not whether you avoid this failure, because you won't. It's whether you let it harden or shame you into inaction, or whether you learn from it; whether you choose to persevere. - Barack Obama",
            "First, have a definite, clear practical ideal; a goal, an objective. Second, have the necessary means to achieve your ends; wisdom, money, materials, and methods. Third, adjust all your means to that end. - Aristotle",
            "Twenty years from now you will be more disappointed by the things that you didn't do than by the ones you did do. So, throw off the bowlines, sail away from safe harbor, catch the trade winds in your sails. Explore, Dream, Discover. - Mark Twain",
            "People who succeed have momentum. The more they succeed, the more they want to succeed and the more they find a way to succeed. Similarly, when someone is failing, the tendency is to get on a downward spiral that can even become a self-fulfilling prophecy. - Tony Robbins"
        }
    },
    {
        "română",{"Soarta este scuza celor slabi și opera celor tari. - Nicolae Titulescu",
            "Adevărul învinge, indiferent de soarta celor care l-au servit. - Gheorghe I. Brătianu",
            "Teoriile nu-s decât mostre fără de valoare. Numai fapta contează. - Constantin Brâncuși",
            "O viață avem români… și-o cinste! Deșteptați-vă că am dormit destul! - Mihai Viteazul",
            "Pentru a judeca drept, nu asculta numai toate vocile, ci ascultă și toate tăcerile. - R. Wurmbrand",
            "Un popor care nu îsi cunoaște istoria este ca un copil care nu îsi cunoaște părintii. - Nicolae Iorga",
            "Nu locuim într-o țară, locuim într-o limbă. Patria asta înseamnă și nimic altceva. - Emil Cioran",
            "Eroismul este o atitudine morală alcatuită din aceeași plămadă ca și sacrificiul de sine. - Dimitrie Gusti",
            "Unirea tuturor românilor într-un singur stat este cea mai fireasca pretenție a civilizației. - Vasile Goldis",
            "Pentru mine, limba română e distanța dintre inimă și umbra ei, care se numește suflet. - Fănuș Neagu",
            "Puterea și fericirea unui stat se află în puterea și fericirea mulțimii, adică a nației. - Mihail Kogalniceanu",
            "Dacă visul unora a fost să ajungă în Cosmos, eu viața întreagă am visat să trec Prutul.  - Grigore Vieru",
            "Prefer să mor în mocirlă într-o Românie Mare decât să mor în paradisul unei Românii mici. - Ion Antonescu",
            "Tancurile sovietice ne-au ocupat țara, comunismul a vrut să ne invadeze și să ne mutileze sufletele. - Dan Puric",
            "Țara noastră nu are ce face cu tineri îmbătrâniți; ei îi trebuiesc bătrâni, mulți bătrâni, însă tineri. - Tudor Arghezi",
            "Pentru a trăi o viața care ți-a fost dată ești dator să o meriți atât ca om cât și ca neam în fiecare clipă. - Mihai Viteazul",
            "Oamenii de geniu sunt făcliile luminoase care se aprind în calea omenirii: fără ei ne-am pierde în întuneric. - A.C. Cuza",
            "Noi suntem, sau am fost, unul din puținele neamuri europene care am experimentat contemplația în suferință. - Mircea Eliade",
            "Civilizația este fructul unei bătălii neîncetate și în orice bătălie înfrângerea e posibilă, la fel ca și victoria. - Spiru Haret",
            "Pe cei care nu-i poți face să cunoască adevărul, învață-i cel puțin să nu-l disprețuiască. - Monseniorul Vladimir Ghika",
            "Ilie Vasca, din Comuna Patrauti, a spus odată, în lagăr: \"Românul n-a stat în genunchi niciodată, decât în fața lui Dumnezeu\"",
            "Literatura, ca și florile, ca și holdele, este o interferență divină între darurile cerului și condițiile pământului. - Gala Galaction",
            "Atunci când justiția nu reușește să fie o formă de memorie, memoria singură poate să fie o formă de justiție - Ana Blandiana",
            "Jur în fața lui Dumnezeu, pe conștiință și onoare că îmi voi jertfi viața pentru triumful cauzei comune românesti… ! - Iuliu Maniu",
            "Cine nu și-a scris istoria cu sângele, acela sau n-a avut-o nicicând, sau crede că poate trăi pe contul istoriei altora. - Grigore Vieru",
            "A fi patriot nu e un merit, e o datorie. Numai cine nu socoate iubirea de Țară drept o datorie e în stare să se laude cu ea. - George Topârceanu",
            "Naționalitatea și limba sunt mai scumpe decât libertatea, căci libertatea pierdută se poate recâștiga, dar naționalitatea niciodată - Gheorghe Barițiu",
            "Mă mișc între Dumnezeu și neamul din care fac parte. În afară de acești termeni, nu văd nimic semnificativ între cer și pământ - Petre țuțea",
            "Citește! Citind mereu, creierul tău va deveni un laborator de idei și imagini, din care vei întocmi înțelesul și filosofia vieții. - Mihai Eminescu",
            "Patriotismul nu este numai iubirea pământului în care te-ai născut ci, mai ales, iubirea trecutului, fără de care nu există iubire de țară.  - Mihai Eminescu",
            "Viața popoarelor este o veșnică și încordată luptă; o luptă pentru drepturi și pentru dreptate, o luptă pentru afirmare și înălțare. - Ion Antonescu",
            "Unirea e singura stare politică ce putea să asigure viitorul nostru și să ne permită a da țării organizarea ce o așteaptă de atât de mult timp. - Alexandru Ioan Cuza",
            "Ce este omul? O ființă creată de Dumnezeu din nemărginita iubire a lui Dumnezeu, căreia i-au fost puse în față fericirea și moartea, spre a alege. - Valeriu Gafencu",
            "Trăim cu frenezie o flecăreală continuă, mulțumiți că avem ce bârfi, fără să ne pese că țara în care locuim ar putea fi părăsită de Istorie. - Octavian Paler",
            "Câteodată, datoria noastră în fața unui adevărat mister nu e să-l lămurim, ci să-l adâncim așa de mult, încât să-l prefacem într-un mister și mai mare. - Lucian Blaga",
            "Istoria pentru noi trebuie să fie un izvor nesecat de poezie sănătoasă, un șir de icoane sfinte de unde să căpătăm totdeauna învățătura de adevăr și insuflare de virtute. - Ion Luca Caragiale",
            "Nu poate fi fericire fără libertate, nu poate fi libertate fără putere, și noi românii nu vom putea fi puternici până când nu ne vom uni cu toții într-unul și același corp politic. - Nicolae Bălcescu",
            "Naționalismul este singura doctrină salvatoare pentru un stat începător ca al nostru. Nu se poate concepe liberul parcurs al tuturor forțelor distructive în mijlocul poporului nostru. - Octavian Goga",
            "Unicul dor al vieții mele e să-mi văd Națiunea mea fericită, pentru care după puteri am și lucrat până acuma, durere fără mult succes, ba tocma acuma cu întristare văd, că speranțele mele și jertfa se prefac în nimica - Avram Iancu",
            "Eram numiți vag, cât mai vag, \"trădătorii poporului -, fără să se specifice despre care popor era vorba […] În veacul veacului nu mă voi rușina de ceea ce am trudit pentru țara mea\" - Nichifor Crainic",
            "Noi vom muri, la fel ca toți cei care azi luptă-n munți. Moartea noastră însă va rămâne o mărturie că neamul acesta s-a opus tiraniei comuniste, că noi am iubit mai mult ca orice libertatea. - testamentul lui Ion Mogoș",
            "Dacă vrăjmașul vostru vă va strâmtora prin rușinoase închinări, atunci mai bine treceți prin paloșul lui decât să fiți privitori ai împilării și ticăloșirii țării voastre - ștefan cel Mare, citat de Vasile Motrescu",
            "Fii om, fii drept și recunoaște că, pe deasupra ambițiilor, intrigilor și urilor, este Patria, este veșnicia neamului, și că acolo trebuie să ne întâlnim totdeauna, chiar dacă nu ne înțelegem de fiecare dată. - Ion Antonescu",
            "Într-o țară așa de frumoasă, cu un trecut așa de glorios, în mijlocul unui popor atât de deștept, cum să nu fie o adevarată religie iubirea de patrie și cum să nu-ți ridici fruntea, ca falnicii strămoși de odinioară, mândru că poți spune: -Sunt român! - Alexandru Vlahuță"
        }
    }
};
}
