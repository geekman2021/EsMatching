<?php

    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;

    class Importer_Telma extends CI_Controller {

        public function __construct() {
            parent::__construct();
        }


        public function index () {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/telma-form");
        }

        public function importer() {
            
            
            if ($_FILES && $_FILES['telma']['name'] && $_FILES["igor"]["name"]) {
                $file_path = $_FILES['telma']['tmp_name'];
                $telma= array();
                $igor_read= $this->readData($_FILES["igor"]["name"], $_FILES["igor"]["tmp_name"]);
                
                if (($handle = fopen($file_path, 'r')) !== FALSE) {
                    while (($row = fgetcsv($handle, 1000, ',')) !== FALSE) {
                        if (count($row) == 20) {
                            $telma[] = array(
                                'date_d' => $row[0],
                                'trans_id' => $row[1],
                                'initiator' => $row[2],
                                'TYPE' => $row[3],
                                'Channel' => $row[4],
                                'State' => $row[5],
                                'Wallet' => $row[6],
                                'Amount_MGA' => str_replace(',', '', $row[7]), // Supprimez les virgules
                                'Sender' => $row[8],
                                'Sender_name' => $row[9],
                                'receiver' => $row[10],
                                'receiver_name' => $row[11],
                                'details1' => $row[12],
                                'Confirming_agent' => $row[13],
                                'notify_alternate' => $row[14],
                                'origine' => $row[15],
                                'TVA' => $row[16],
                                'ACTION' => $row[17],
                                'AA1_GROUP' => $row[18],
                                'PAR' => $row[19]
                            );
                            
                            // Insérez les données dans la base de données
                            // $this->Transaction_model->insert_transaction($data);
                        }
                    }
                    fclose($handle);

                    if (count($igor_read) > 1 ) {
                
                        for($i=1; $i < count($igor_read); $i++) {
                            $montant= str_replace(".00", "", $igor_read[$i][4]);

                            $igor[]= array(
                               "COMPTE" => $igor_read[$i][0],
                               "DATE_OPER" =>  $igor_read[$i][1],
                               "DATE_VAL" => $igor_read[$i][2],
                               "DEVISE" => $igor_read[$i][3],
                               "MONTANT" => $montant,
                               "LIBELLE" => $igor_read[$i][5],
                               "OPER" => $igor_read[$i][6],
                               "EXPL" => $igor_read[$i][7],
                               "REF_IGOR" => $igor_read[$i][8],
                            );
                        }
                    }


                    $telma = $this->supprimerEspace($telma);
                    $telma = $this->trierParTransId($telma);

                    $reverseEtCle= $this->trouverIndexEtCleReverse($telma);

                    $indexAnnul= $this->trouverAnnulation($telma, $reverseEtCle);
                    $indexAdmin= $this->trouverIndexAdmin($telma);
                    $indexRollback = $this->trouverIndexRollBack($telma);

                    $indexSupprimer= array_merge($indexAnnul, $indexAdmin);

                    $indexSupprimer= array_merge($indexSupprimer, $indexRollback);

                    $reste= $this->supprimerIndices($telma, $indexSupprimer);
                    $admin = $this->filtrerAdmin($telma);
                    $reverse= $this->filtrerReverse($telma);

                    $rollback= $this->filtrerRollBack($telma);

                    $telmaCI= $this->filtrerCI($reste);
                    $telmaCI= $this->trierParCleCroissante($telmaCI);

                    $telmaCO= $this->filtrerCO($reste);
                    // $telmaCO= $this->trierParCleCroissante($telmaCO);
                    // echo "<<<<<<<<<<<<ROLLBACK>>>>>>>>>>>>>>>>>>>>>>>\n";


                    $igor = $this->supprimerEspace($igor);
                    $igorCI= $this->filtrerCIIgor($igor);
                    $igorCI= $this->trierParCleCroissante($igorCI);
                    $igorCO= $this->filtrerCOIgor($igor);
                    $igorCO= $this->trierParCleCroissante($igorCO);

                    // if ($this->estTrieParCle($telmaCI)) {
                    //     echo "Le tableau est trié par ordre croissant en se référant à la clé 'cle'.";
                    // } else {
                    //     echo "Le tableau n'est pas trié par ordre croissant en se référant à la clé 'cle'.";
                    // }

                    // echo "<pre>";
                    //     print_r($igorCI);
                    // echo "</pre>";

                    $this->comparerTelmaEtIgor($igorCI, $telmaCI);
                }

                
            }
        }

        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension ==="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

        }


        public function supprimerEspace($data) {
            $resultat= array();
            foreach($data as $key=> $valeur) {
                $resultat[$key] = str_replace(" ", "", $valeur);
            }

            return $resultat;
        }

        public function filtrerVI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["ref_rel"] ==="APPROUME" && $item["OPER"] ==="VI") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerAdmin($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["TYPE"] ==="adjustment") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerReverse($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["TYPE"] ==="reverse") {
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerRollBack($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["TYPE"] ==="transfer" && $item["Channel"] ==="SOAP" && $item["State"] ==="RolledBack") {
                    $resultat[]= $item;
                } 
            }

            return $resultat;
        }

        // public function filtrerSoapEtCompltedEtTransfer($data) {
        //     $resultat= array();

        //     foreach($data as $item) {
        //         if($item["TYPE"] ==="transfer" && $item["Channel"] ==="SOAP" && $item["State"] ==="Completed") {
        //             $resultat[]= $item;
        //         }
        //     }

        //     return $resultat;
        // }

        public function filtrerCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["ACTION"] ==="bank_to_wallet") {
                    $item["cle"] = $item["receiver"] .$item["Amount_MGA"];
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerCO($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["ACTION"] ==="wallet_to_bank") {
                    $item["cle"] = $item["Sender"] .$item["Amount_MGA"];
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function trierParCleCroissante($data) {

            usort($data, function ($a, $b) {
                $cleA = intval($a['cle']);
                $cleB = intval($b['cle']);

                if ($cleA == $cleB) {
                    return 0;
                }
                return ($cleA < $cleB) ? -1 : 1;
            });

            return $data;

        }

        public function trierParTransId($data) {
            usort($data, function($a, $b) {
                return $a['trans_id'] - $b['trans_id'];
            });
            return $data;
        }

        public function trouverIndexEtCleReverse($data) {
            $indexesEtCleReverse = [];
            $resultat= array();
            foreach ($data as $index => $element) {
                if ($element['TYPE'] === 'reverse') {
                    $indexesEtCleReverse["index"] = $index;
                    $indexesEtCleReverse["receiver"] = $element["receiver"];
                    $resultat[]= $indexesEtCleReverse;

                }
            }
            return $resultat;
        }

        public function trouverAnnulation($data, $indexesEtCleReverse) {

            $resultatIndex = array();
            $indexAnnul;

            foreach($indexesEtCleReverse as $itemCle) {

                $i=0;
                $indexAnnul =0;
                for ($i ; $i < $itemCle["index"]  ; $i++) {
                    if($data[$i]["initiator"] === $itemCle["receiver"]) {
                        $indexAnnul=$i;
                    } 
                }
                $resultatIndex[] = $indexAnnul;
                $resultatIndex[] = $itemCle["index"];
            }

            // $resultatIndex[]= array_merge($resultatIndex, $indexesEtCleReverse["index"]);

            return $resultatIndex;
        }


        public function trouverIndexAdmin($data) {
            
            $index= array();
            $i=0;

            for ($i=0; $i <count($data); $i++) {
                if($data[$i]["TYPE"] ==="adjustment") {
                    $index[]= $i;
                }
            }
            return $index;
        }

        public function trouverIndexRollBack($data) {
            $index= array();
            $i=0;

            for ($i=0; $i <count($data); $i++) {
                if($data[$i]["TYPE"] ==="transfer" && $data[$i]["Channel"] ==="SOAP" && $data[$i]["State"] ==="RolledBack") {
                    $index[]= $i;
                }
            }
            return $index;
        }


        public function supprimerIndices($telma, $indicesASupprimer) {
            foreach ($indicesASupprimer as $indice) {
                if (isset($telma[$indice])) {
                    unset($telma[$indice]);
                }
            }
            return array_values($telma);
        }


        public function filtrerCIIgor($data) {

            $resultat= array();
            foreach($data as $item) {
                if($item["EXPL"] ==="AU" && $item["OPER"] ==="CASHI") {
                    $libelle= $item["LIBELLE"];
                    if (preg_match('/tel:(\d+)/', $libelle, $matches)) {
                        $numeroTelephone = $matches[1];
                        $item["cle"] = $numeroTelephone .$item["MONTANT"]; 
                        $resultat[]= $item;                       
                    }
                }
            }
            return $resultat;
        }

        public function filtrerCOIgor($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["EXPL"] ==="AU" && $item["OPER"] ==="CASHO") {
                    $libelle= $item["LIBELLE"];
                    if (preg_match('/tel:(\d+)/', $libelle, $matches)) {
                        $numeroTelephone = $matches[1];
                        $item["cle"] = $numeroTelephone .-($item["MONTANT"]); 
                        $resultat[]= $item;                       
                    }
                }
            }
            return $resultat;
        }

        public function estTrieParCle($data) {
            $longueur = count($data);
            
            for ($i = 0; $i < $longueur - 1; $i++) {
                if ($data[$i]['cle'] > $data[$i + 1]['cle']) {
                    return false; // Le tableau n'est pas trié
                }
            }
            
            return true; 
        }


        public function comparerTelmaEtIgor($igor,$telma) {
            
            $ligne= count($igor) > count($telma) ? count($telma) : count($igor);
            $telmaNormale = array();
            $igorNormale= array();
            $igorAnomalie= array();
            $telmaAnomalie = array();

            $igorCopy =$igor;

            $i=0;
            while ($i <=$ligne) {
                if($igorCopy[$i]["cle"] - $telma[$i]["cle"] === 0) {
                   $telmaNormale[] = $telma[$i];
                   $igorNormale[] = $igor[$i];
                   $i++;
                } else if ($igorCopy[$i]["cle"] < $telma[$i]["cle"]) {
                    // array_push($igorAnomalie, array_splice($igor[$i], $i, 1));
                    // array_splice($igorCopy[$i], $i, 1);
                    unset($igorCopy[$i]);
                    array_values($igorCopy);
                    echo "<pre>";
                        print_r($igorCopy);
                    echo "</pre>";

                    break;

                    echo "---------------------------------------------" .$i;
                    // echo "mandalo"; 
                } else {
                    
                }
            }

            // echo "<pre>";
            //     print_r($igor);
            // echo "</pre>";

            // return [[$igorNormale, $telmaNormale], [$igorAnomalie, $telmaAnomalie]];
        }

        // public function comparerTelmaEtIgor($igor, $telma) {
        //     $ligne = min(count($igor), count($telma)); // Utilisez min() pour obtenir la plus petite longueur
        
        //     $telmaNormale = [];
        //     $igorNormale = [];
        //     $igorAnomalie = [];
        //     $telmaAnomalie = [];
        
        //     for ($i = 0; $i < $ligne; $i++) {
        //         if ($igor[$i]["cle"] === $telma[$i]["cle"]) {
        //             $telmaNormale[] = $telma[$i];
        //             $igorNormale[] = $igor[$i];
        //         } elseif ($igor[$i]["cle"] < $telma[$i]["cle"]) {
        //             $igorAnomalie[] = $igor[$i];
        //         } else {
        //             $telmaAnomalie[] = $telma[$i];
        //         }
        //     }
        
        //     echo "<pre>";
        //     print_r($igorAnomalie);
        //     echo "</pre>";
        
        //     // return [[$igorNormale, $telmaNormale], [$igorAnomalie, $telmaAnomalie]];
        // }
        
        

    }



?>