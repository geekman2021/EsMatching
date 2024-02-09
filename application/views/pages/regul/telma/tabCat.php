
<div id="tabCat" class="container-fluid mt-1 tab-pane "><br>  
        <!-- <div class="row">
            <div class="col-sm-6 pl-4">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6 pr-4">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div>  -->
        <div class="container-fluid mt-5">
            <div class="card">
                <div class="card-header">
                    <b>Liste Credit à tort regularisé</b>
                </div>
                <div class="card-body">
                <table class="table table-bordered" id="tableCat">
                <thead style="text-align: center ;">
                    <tr>
                        <th>Compte</th>
                        <th>Date_Oper</th>
                        <th>Date_Val </th>
                        <th>Montant</th>
                        <th>Devise</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody>
                    <!-- <tr> -->
                        <!-- <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalCat"><i class="bx bx-show" style="font-size: 20px"></i></a></td> -->
                    <!-- </tr> -->

                </tbody>
            </table>
                </div>
            </div>
            
        </div>
    </div>
    <?php $this->load->view("pages/regul/telma/modal/modalCat"); ?>